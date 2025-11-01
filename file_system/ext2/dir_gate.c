#include "dir_gate.h"

__u32 ext2_dir_entry_init(struct ext2_dir_entry *dentry, void* ptr) {
    *dentry = *(struct ext2_dir_entry*)ptr;
    dentry->name = malloc(sizeof(__u8) * dentry->name_len);
    int i;
    for (i = 0; i < dentry->name_len; i++) {
        dentry->name[i] = *(__u8*)(ptr + sizeof(__u64) + i);
    }
    return 0;
}
__u32 ext2_dir_entry_destroy(struct ext2_dir_entry *dentry) {
    free(dentry->name);
}

static __u32 alloc_new_inode_gate_at_dir(struct dir_gate *dg, struct ext2_file_system* fs, __u32 inode) {
    struct inode *i = malloc(sizeof(struct inode));
    *i = read_inode(fs, inode);
    inode_gate_init(&(dg->ig), fs, i);
    dg->id = inode;
}

static __u32 free_inode_gate_and_cache_at_dir(struct dir_gate *dg) {
    free(dg->ig.inode);
    if (dg->current_block != NULL) { block_munmap(dg->ig.fs, dg->current_block); };
    put_inode(dg->ig.fs, *dg->ig.inode, dg->id);
    if (inode_gate_destroy(&(dg->ig)) != 0) { return 1; }
    return 0;
}

__u32 dir_gate_init(struct dir_gate *dg, struct ext2_file_system *fs, __u32 inode) {
    alloc_new_inode_gate_at_dir(dg, fs, inode);

    dir_link_init(&dg->dl);
    dg->offset = 0;
    dg->current_block = block_mmap(fs, get_current_block_id(&dg->ig));
    return 0;
}
__u32 dir_gate_destroy(struct dir_gate *dg) {
    free_inode_gate_and_cache_at_dir(dg);
    dir_link_destroy(&dg->dl);
    return 0;
}

__u32 next_entry(struct dir_gate *dg) {
    struct ext2_dir_entry entry = *(struct ext2_dir_entry*)(dg->current_block + dg->offset);
    dg->offset += entry.rec_len;
    dir_link_add_value(&dg->dl, entry.rec_len, 0);

    if (dg->offset == get_block_size_from_fs(dg->ig.fs)) {
        if (dg->ig.inode->i_size == get_current_block_number(&(dg->ig)) + 1) {
            return 1;
        }
        block_munmap(dg->ig.fs, dg->current_block);
        next_block(&dg->ig);
        dg->current_block = block_mmap(dg->ig.fs, get_current_block_id(&dg->ig));
        dg->offset = 0;
    }
    return 0;
}
__u32 entry_current_dir(struct dir_gate *dg) { // todo: review
    struct ext2_dir_entry entry = *(struct ext2_dir_entry*)(dg->current_block + dg->offset);
    if (entry.file_type != 2) { return 1; }
    dir_link_add_value(&dg->dl, dg->id, 1);

    struct ext2_file_system *fs = dg->ig.fs;

    free_inode_gate_and_cache_at_dir(dg);

    alloc_new_inode_gate_at_dir(dg, fs, entry.inode);

    dg->offset = 0;
    dg->current_block = block_mmap(fs, get_current_block_id(&dg->ig));
    return 0;
}

__u32 prev_step(struct dir_gate *dg) {
    if (dg->dl.head == 0) { return 1; }
    if (dir_link_is_id(&dg->dl)) {
        __u32 id = dir_link_get_value(&dg->dl);
        struct ext2_file_system *fs = dg->ig.fs;

        free_inode_gate_and_cache_at_dir(dg);
        alloc_new_inode_gate_at_dir(dg, fs, id);
        dg->offset = 0;
        dg->current_block = block_mmap(fs, get_current_block_id(&dg->ig));

        dir_link_remove_value(&dg->dl);
        // WARNING: IGNORING ABSTRACTION!!!
        __u8 is_id_flag = dir_link_is_id(&dg->dl);
        struct dir_link_node *ptr = dg->dl.head;
        while (!is_id_flag) {
            dg->offset += ptr->value;
            ptr = ptr->prev;
            is_id_flag = ptr->is_id;
        }
        return 0;
    }
    else {
        if (dg->offset == 0) {
            dg->offset = get_block_size_from_fs(dg->ig.fs);
            block_munmap(dg->ig.fs, dg->current_block);
            prev_block(&dg->ig);
            dg->current_block = block_mmap(dg->ig.fs, get_current_block_id(&dg->ig));
        }
        dg->offset -= dir_link_get_value(&dg->dl);
        dir_link_remove_value(&dg->dl);
        return 0;
    }

}

struct ext2_dir_entry get_current_entry(struct dir_gate *dg) {
    struct ext2_dir_entry dentry;
    ext2_dir_entry_init(&dentry, dg->current_block + dg->offset);
    return dentry;
}

__u32 delete_current_entry(struct dir_gate *dg) {
    struct ext2_dir_entry entry = *(struct ext2_dir_entry*)(dg->current_block + dg->offset);
    if (entry.name_len == 1 && entry.name[0] == '.' ||
    entry.name_len == 2 && entry.name[0] == '.' && entry.name[1] == '.') { return 1; }
    __u32 cur_len = entry.rec_len;

    dg->offset -= dir_link_get_value(&dg->dl);
    dir_link_remove_value(&dg->dl);

    struct ext2_dir_entry *dentry = (struct ext2_dir_entry*)(dg->current_block + dg->offset);
    dentry->rec_len += cur_len;
    return 0;

}

static __u32 get_name_size_in_alloc_bytes(const char *name) {
    int i = 0;
    while(name[i] != 0) { i++; }
    int bytes = CEIL_DIV(i, sizeof(__u32));
    return i;
}
static __u32 get_real_rec_len(const char *name) {
    return sizeof(__u32) * (2 + get_name_size_in_alloc_bytes(name));
}
static __u32 get_real_rec_len_with_len(__u32 name_len) {
    return sizeof(__u32) * (2 + CEIL_DIV(name_len, sizeof(__u32)));
}

static void enter_new_entry_at_pointer(__u32 inode, __u16 rec_len, __u8 file_type, const char *name, void *ptr, __s32 rest) {
    *(__u32*)ptr = inode;
    *(__u16*)(ptr + sizeof(__u32)) = rec_len + rest;
    __u8 i = 0;
    while(name[i] != 0) {
        *(__u8*)(ptr + 2 * sizeof(__u32) + i) = name[i];
        i++; 
    }
    int j;
    for (j = i; j < sizeof(__u32) * CEIL_DIV(i, sizeof(__u32)); j++) {
        *(__u8*)(ptr + 2 * sizeof(__u32) + j) = 0;
    }
    *(__u8*)(ptr + sizeof(__u32) + sizeof(__u16)) = i;
    *(__u8*)(ptr + sizeof(__u32) + sizeof(__u16) + sizeof(__u8)) = file_type;

}
__u32 add_new_entry(struct dir_gate *dg, const char *name,__u8 file_type, __u32 inode) {
    
    struct inode_gate ig;
    inode_gate_init(&ig, dg->ig.fs, dg->ig.inode);
    void *ptr = block_mmap(ig.fs, get_current_block_id(&ig));
    struct ext2_dir_entry entry = *(struct ext2_dir_entry*)ptr;
    __u32 req_rec_len = get_real_rec_len(name);
    __u32 offset = 0;
    __u32 block_offset = 0;
    __u32 total_size_in_blocks = ig.inode->i_size / get_block_size_from_fs(ig.fs);
    while (block_offset < total_size_in_blocks) {
        while (offset < get_block_size_from_fs(ig.fs)) {
            __s32 rest = entry.rec_len - (get_real_rec_len_with_len(entry.name_len) + req_rec_len);
            if (rest > 0) {
                *(__u16*)(ptr + offset + sizeof(__u32)) -= rest;
                enter_new_entry_at_pointer(inode, req_rec_len, file_type, name, ptr + offset, rest);
                block_munmap(ig.fs, ptr);
                inode_gate_destroy(&ig);
                return 0;
            }
            offset += entry.rec_len;
            if (offset != get_block_size_from_fs(ig.fs)) {
                entry = *(struct ext2_dir_entry*)(ptr + offset);
            }
        }
        offset = 0;
        block_offset++;
        if (block_offset != total_size_in_blocks) {
            block_munmap(ig.fs, ptr);
            next_block(&ig);
            ptr = block_mmap(ig.fs, get_current_block_id(&ig));
            entry = *(struct ext2_dir_entry*)ptr;
        }
    }
    __u32 new_block = first_free_block(ig.fs, get_current_block_number(&ig));
    block_alloc(ig.fs, new_block);
    append_block(&ig, new_block);

    block_munmap(ig.fs, ptr);
    next_block(&ig);
    ptr = block_mmap(ig.fs, get_current_block_id(&ig));
    entry = *(struct ext2_dir_entry*)ptr;
    enter_new_entry_at_pointer(inode, req_rec_len, file_type, name, ptr + offset, get_block_size_from_fs(ig.fs) - req_rec_len);
    block_munmap(ig.fs, ptr);
    inode_gate_destroy(&ig);
    return 0;
}