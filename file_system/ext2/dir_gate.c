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

__u8 has_next_entry(struct dir_gate *dg) {
    return get_current_block_number(&dg->ig) + 1 < get_real_size_in_alloc_blocks(&dg->ig);
}

__u32 next_entry(struct dir_gate *dg) {
    struct ext2_dir_entry entry = *(struct ext2_dir_entry*)(dg->current_block + dg->offset);
    dg->offset += entry.rec_len;
    dir_link_add_value(&dg->dl, entry.rec_len, 0);

    if (dg->offset == get_block_size_from_fs(dg->ig.fs)) {
        if (!has_next_entry(dg)) {
            prev_step(dg);
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

static void release_one_hard_link_of_inode(struct ext2_file_system *fs, __u32 id) {
    struct inode inode = read_inode(fs, id);
    inode.i_links_count--;
    if (inode.i_links_count == 0) {
        free_inode(fs, id);
    }
    else {
        put_inode(fs, inode, id);
    }
}

__u32 delete_current_entry(struct dir_gate *dg) {
    struct ext2_dir_entry entry = *(struct ext2_dir_entry*)(dg->current_block + dg->offset);
    if (entry.name_len == 1 && entry.name[0] == '.' ||
    entry.name_len == 2 && entry.name[0] == '.' && entry.name[1] == '.') { return 1; }
    __u32 cur_len = entry.rec_len;
    release_one_hard_link_of_inode(dg->ig.fs, entry.inode);
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
    return bytes;
}
static __u32 get_real_rec_len(const char *name) {
    return sizeof(__u32) * (2 + get_name_size_in_alloc_bytes(name));
}
static __u32 get_real_rec_len_with_len(__u32 name_len) {
    return sizeof(__u32) * (2 + CEIL_DIV(name_len, sizeof(__u32)));
}

static __u8 is_current_entry_addable(struct dir_gate *dg, __u32 rec_len) {
    struct ext2_dir_entry dentry = *(struct ext2_dir_entry*)(dg->current_block + dg->offset);
    return dentry.rec_len >= get_real_rec_len_with_len(dentry.name_len) + rec_len;
}

static void enter_new_entry_at_pointer(__u32 inode, __u8 file_type, const char *name, void *new_entry_ptr, __s32 rest) {
    *(__u32*)(new_entry_ptr) = inode;
    *(__u16*)(new_entry_ptr + sizeof(__u32)) = rest;
    *(__u8*)(new_entry_ptr + sizeof(__u32) + sizeof(__u16) + sizeof(__u8)) = file_type;
    __u32 i = 0;
    while(name[i] != 0) {
        *(__u8*)(new_entry_ptr + 2 * sizeof(__u32) + i) = name[i];
        i++; 
    }
    *(__u8*)(new_entry_ptr + sizeof(__u32) + sizeof(__u16)) = i;
    int j;
    for (j = i; j < sizeof(__u32) * CEIL_DIV(i, sizeof(__u32)); j++) {
        *(__u8*)(new_entry_ptr + 2 * sizeof(__u32) + j) = 0;
    }
}

static __u32 add_new_entry_to_current_entry(void* ptr, const char *name, __u8 file_type, __u32 inode) {
    __u32 old_rest = ((struct ext2_dir_entry*)(ptr))->rec_len - get_real_rec_len_with_len(((struct ext2_dir_entry*)(ptr))->name_len);
    ((struct ext2_dir_entry*)(ptr))->rec_len -= old_rest;
    void *new_entry_ptr = ptr + ((struct ext2_dir_entry*)ptr)->rec_len;
    enter_new_entry_at_pointer(inode, file_type, name, new_entry_ptr, old_rest);
}

static void add_one_hard_link_of_inode(struct ext2_file_system *fs, __u32 id) {
    struct inode inode = read_inode(fs, id);
    inode.i_links_count++;
    put_inode(fs, inode, id);
}

__u32 add_new_entry(struct ext2_file_system *fs, __u32 dir, const char *name, __u8 file_type, __u32 inode) {
    struct dir_gate dg;
    dir_gate_init(&dg, fs, dir);
    for (; get_current_block_number(&dg.ig) < get_real_size_in_alloc_blocks(&dg.ig); next_entry(&dg)) {
        if (is_current_entry_addable(&dg, get_real_rec_len(name))) {
            add_new_entry_to_current_entry(dg.current_block + dg.offset, name, file_type, inode);
            add_one_hard_link_of_inode(fs, inode);
            dir_gate_destroy(&dg);
            return 0;
        }
    }
    __u32 hint = get_current_block_number(&dg.ig) + 1;
    __u32 block = first_free_block(fs, hint);
    block_alloc(fs, block);
    append_block(&dg.ig, block);

    next_entry(&dg);
    enter_new_entry_at_pointer(inode, file_type, name, dg.current_block, get_block_size_from_fs(fs));
    add_one_hard_link_of_inode(fs, inode);

    dir_gate_destroy(&dg);
    return 0;
}