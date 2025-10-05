#pragma once
#include "types.h"

struct block_group_descriptor_table {
    __u32 bg_block_bitmap; /* block id of the first block of the “block bitmap” for the group represented. */
    __u32 bg_inode_bitmap; /* block id of the first block of the “inode bitmap” for the group represented */
    __u32 bg_inode_table; /* block id of the first block of the “inode table” for the group represented */
    __u16 bg_free_blocks_count; /* Free blocks count in group */
    __u16 bg_free_inodes_count; /* Free inodes count in group */
    __u16 bg_used_dirs_count; /*  indicating the number of inodes allocated to directories */
    __u16 bg_pad; /* Padding */
    __u32 bg_reserved[3]; /* Reserved */
};