#pragma once
#include "inode_gate.h"
#include <string.h>

struct file_desc {
    struct inode_gate ig;
    __u32 inode_id;
    
    __u32 cur_offset;
    __u32 flags;

    __u32 block_size;
};

__u32 fd_init(struct file_desc *fd, struct ext2_file_system *fs, struct inode *inode, __u32 inode_id, __u32 flags);
__u32 fd_destroy(struct file_desc *fd);

__u32 fd_move_by_offset(struct file_desc *fd, __u32 offset);

__u32 fd_read(struct file_desc *fd, __u32 size, void *buf);
__u32 fd_write(struct file_desc *fd, __u32 size, void *buf);

__u32 fd_ftruncate(struct file_desc *fd, __u32 length);