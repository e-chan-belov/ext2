#pragma once

#include "ext2_fs.h"

struct virtual_inode {
    struct ext2_file_system *fs;
    struct inode *inode;

    __u8 direct_index;
    __u32 first_index;
    __u32 second_index;
    __u32 third_index;

    __u32 *first_block;
    __u32 *second_block;
    __u32 *third_block;
};

void virtual_inode_init(struct virtual_inode *vi, struct ext2_file_system *fs, struct inode *inode);
void virtual_inode_destroy(struct virtual_inode *vi);

void next_block(struct virtual_inode *vi);
void prev_block(struct virtual_inode *vi);
void to_first_block(struct virtual_inode *vi);

__u32 is_current_block_used(struct virtual_inode *vi);
__u32 get_block_id(struct virtual_inode *vi);