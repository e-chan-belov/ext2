#include "virtual_inode.h"

void virtual_inode_init(struct virtual_inode *vi, struct ext2_file_system *fs, struct inode *inode) {
    vi->fs = fs;
    vi->inode = inode;

    vi->direct_index = 0;
    vi->first_index = 0;
    vi->second_index = 0;
    vi->third_index = 0;

    vi->first_block = NULL;
    vi->second_block = NULL;
    vi->third_block = NULL;
}

void virtual_inode_destroy(struct virtual_inode *vi) {
    if (vi->first_block != NULL) { block_munmap(vi->fs, vi->first_block); }
    if (vi->second_block != NULL) { block_munmap(vi->fs, vi->second_block); }
    if (vi->third_block != NULL) { block_munmap(vi->fs, vi->third_block); }
}

void next_block(struct virtual_inode *vi) {
    if (vi->direct_index < 12) {
        vi->direct_index++;
    }
    else if (vi->direct_index == 12) {
        
    }
    
}