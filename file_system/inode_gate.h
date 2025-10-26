#pragma once

#include <ext2_fs.h>

#define DIRECT_INDEXES_AMOUNT 12
#define FIRST_INDIRECT_BLOCK_ID 12
#define SECOND_INDIRECT_BLOCK_ID 13
#define THIRD_INDIRECT_BLOCK_ID 14

#define BLOCKS_SIZE_IN_I_BLOCKS 512

struct inode_gate {
    struct ext2_file_system *fs;
    struct inode *inode;

    __u32 lc;

    /* cache section */
    __u32 first_indirect_id;
    __u32 *first_indirect_block;
    __u32 second_indirect_id;
    __u32 *second_indirect_block;
    __u32 third_indirect_id;
    __u32 *third_indirect_block;
};

__u32 inode_gate_init(struct inode_gate *ig, struct ext2_file_system *fs, struct inode *inode);
__u32 inode_gate_destroy(struct inode_gate *ig);

struct inode get_inode_copy(struct inode_gate *ig);

__u32 get_real_size_in_blocks(struct inode_gate *ig);
__u32 get_current_block_number(struct inode_gate *ig);

__u32 next_block(struct inode_gate *ig);
__u32 prev_block(struct inode_gate *ig);
__u32 move_by_offset(struct inode_gate *ig, __s32 offset);
__u32 move_to_first_block(struct inode_gate *ig);

__u32 get_current_block_id(struct inode_gate *ig);

__u32 append_block(struct inode_gate *ig, __u32 block);
__u32 unlink_last_block(struct inode_gate *ig);