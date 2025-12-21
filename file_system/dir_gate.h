#pragma once

#include "inode_gate.h"
#include "dir_link.h"

struct ext2_dir_entry {
	__u32	inode;			// Inode number 
	__u16	rec_len;		// Directory entry length
	__u8	name_len;		// Name length 
	__u8	file_type;
	__u8	name[256];			// File name, up to EXT2_NAME_LEN
};

__u32 ext2_dir_entry_init(struct ext2_dir_entry *dentry, void* ptr);
const char* ext2_dir_entry_get_name(struct ext2_dir_entry *dentry);

struct dir_gate {
    struct inode_gate ig;
	__u32 id;

	struct dir_link dl;
    __u32 offset;
    void *current_block;
};

__u32 dir_gate_init(struct dir_gate *dg, struct ext2_file_system *fs, __u32 inode);
__u32 dir_gate_destroy(struct dir_gate *dg);

__u32 next_entry(struct dir_gate *dg);
__u32 entry_current_dir(struct dir_gate *dg);

__u32 prev_step(struct dir_gate *dg);

struct ext2_dir_entry get_current_entry(struct dir_gate *dg);
__u32 delete_current_entry(struct dir_gate *dg);

// warning: destroy active inode_gates before use!!!
__u32 add_new_entry(struct ext2_file_system *fs, __u32 dir, const char *name,__u8 file_type, __u32 inode);