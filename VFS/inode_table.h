#pragma once
#include "inode.h"
#include "ext2_fs.h"
#include "rb_tree.h"

struct inode_table_entry {
    struct inode inode;
    __u32 links_count;
};

struct inode_table {
    struct ext2_file_system *fs;
    struct rb_tree table;
};

__u32 inode_table_init(struct inode_table* it, struct ext2_file_system *fs);
__u32 inode_table_destroy(struct inode_table* it);

struct inode* inode_table_link_inode(struct inode_table *it, __u32 id);
__u32 inode_table_unlink_inode(struct inode_table *it, __u32 id);