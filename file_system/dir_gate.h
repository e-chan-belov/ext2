#pragma once

#include "inode_gate.h"

struct ext2_dir_entry {
	__u32	inode;			// Inode number 
	__u16	rec_len;		// Directory entry length
	__u8	name_len;		// Name length 
	__u8	file_type;
	__u8	name[];			// File name, up to EXT2_NAME_LEN
};

struct dir_gate {
    struct inode_gate ig;

    __u32 offset;
    void *current_block;
};

__u32 dir_gate_init(struct dir_gate *dg, struct ext2_file_system *fs, __u32 inode);
__u32 dir_gate_destroy(struct dir_gate *dg);

__u32 next_entry(struct dir_gate *dg);
__u32 entry_current_dir(struct dir_gate *dg);

struct ext2_dir_entry* get_current_entry(struct dir_gate *dg);
__u32 delete_current_entry(struct dir_gate *dg);
__u32 append_new_entry(struct dir_gate *dg);