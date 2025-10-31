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

__u32 dir_gate_init(struct dir_gate *dg, struct ext2_file_system *fs, __u32 inode) {
    struct inode *i = malloc(sizeof(struct inode));
    *i = read_inode(fs, inode);
    inode_gate_init(&(dg->ig), fs, i);
    dg->id = inode;

    dir_link_init(&dg->dl);
    dg->offset = 0;
    dg->current_block = block_mmap(fs, get_current_block_id(&dg->ig));
    return 0;
}

__u32 dir_gate_destroy(struct dir_gate *dg) {
    free(dg->ig.inode);
    if (dg->current_block != NULL) { block_munmap(dg->ig.fs, dg->current_block); };
    put_inode(dg->ig.fs, *dg->ig.inode, dg->id);
    if (inode_gate_destroy(&(dg->ig)) != 0) { return 1; }
    dir_link_destroy(&dg->dl);
    return 0;
}

__u32 next_entry(struct dir_gate *dg) {
    /* todo */
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

struct ext2_dir_entry get_current_entry(struct dir_gate *dg) {
    struct ext2_dir_entry dentry;
    ext2_dir_entry_init(&dentry, dg->current_block + dg->offset);
    return dentry;
}