#pragma once 

#include "ext2_fs.h"

#include "types.h"
#include "file.h"
/*

 //* Since EXT2 structures are
 //* stored in intel byte order, and the name_len field could never be
 //* bigger than 255 chars, it's safe to reclaim the extra byte for the
 //* file_type field.
 


struct dir {
	struct file *file;
	struct ext2_dir_entry *current_entry;
};

int dir_init(struct dir *dir, struct file *file) {
	if (!IS_DIR(file->inode.i_mode)) { return -1; }
	dir->file = file;
	dir->current_entry = file->ptr;
	return 0;
}

struct ext2_dir_entry get_current_entry(struct dir *dir) {
	return *(dir->current_entry);
}

int next_entry(struct dir *dir) {
	void *ptr = (void*)dir->current_entry;
	__u32 offset = dir->current_entry->rec_len;
	if (((ptr + offset) - dir->file->ptr) >= dir->file->inode.i_size) { return -1; }
	dir->current_entry = (struct ext2_dir_entry*)(ptr + offset);
	return 0;
}
*/

struct ext2_dir_entry {
	__u32	inode;			// Inode number 
	__u16	rec_len;		// Directory entry length
	__u8	name_len;		// Name length 
	__u8	file_type;
	__u8	name[];			// File name, up to EXT2_NAME_LEN
};

struct virtual_dir {

	void *current_block;
	__u32 offset;
};

__u32 virtual_dir_init(struct virtual_dir *vd, struct ext2_file_system *fs, struct inode *inode);
__u32 virtual_dir_destroy(struct virtual_dir *vd);

void virtual_dir_next_entry();
struct ext2_dir_entry get_current_entry();