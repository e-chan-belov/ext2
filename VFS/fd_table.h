#pragma once 

#include "inode.h"
#include "ext2_fs.h"
#include "inode_gate.h"
#include "file_desc.h"
#include "inode_table.h"

#define FIRST_POTENTIAL_FD_NUMBER 3
#define MAX_AMOUNT_OF_FD 1000000

struct fd_table {
    struct rb_tree table;
    struct ext2_file_system *fs;
    struct inode_table *inode_table;
};

__u32 fd_table_init(struct fd_table *fd_table, struct inode_table *inode_table);
__u32 fd_table_destroy(struct fd_table *fd_table);

__u32 fd_table_open(struct fd_table *fd_table, __u32 inode_id, __u32 flags);
__u32 fd_table_close(struct fd_table *fd_table, __u32 fd_id);

__u32 fd_table_move_by_offset(struct fd_table *fd_table, __u32 fd, __u32 offset);

__u32 fd_table_read(struct fd_table *fd_table,__u32 fd, __u32 size, void *buf);
__u32 fd_table_write(struct fd_table *fd_table,__u32 fd, __u32 size, void *buf);

__u32 fd_table_ftruncate(struct fd_table *fd_table,__u32 fd, __u32 rem_offset);