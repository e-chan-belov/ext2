#include "dir_gate.h"

__u32 dir_gate_init(struct dir_gate *dg, struct ext2_file_system *fs, __u32 inode) {
    struct inode *i = malloc(sizeof(struct inode));
    *i = read_inode(fs, inode);
    inode_gate_init(&(dg->ig), fs, i);

    dg->offset = 0;
    dg->current_block = block_mmap(fs, get_current_block_id(&dg->ig));
    return 0;
}

__u32 dir_gate_destroy(struct dir_gate *dg) {
    free(dg->ig.inode);
    inode_gate_destroy(&(dg->ig));
    block_munmap(dg->ig.fs, dg->current_block);
    return 0;
}

__u32 next_entry(struct dir_gate *dg) {
    /* todo */
    struct ext2_dir_entry entry = *(struct ext2_dir_entry*)(dg->current_block + dg->offset);
    dg->offset += entry.rec_len;



    if (dg->offset == get_block_size_from_fs(dg->ig.fs)) {
        if (get_real_size_in_blocks(&(dg->ig)) == get_current_block_number(&(dg->ig)) + 1) {
            return 1;
        }
        block_munmap(dg->ig.fs, dg->current_block);
        next_block(&(dg->ig));
        dg->current_block = block_mmap(dg->ig.fs, get_current_block_id(&dg->ig));
        dg->offset = 0;
    }
    return 0;
}

struct ext2_dir_entry* get_current_entry(struct dir_gate *dg) {
    return (struct ext2_dir_entry*)(dg->current_block + dg->offset);
}