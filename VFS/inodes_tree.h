#pragma once
#include "inode.h"
#include "ext2_fs.h"

enum tree_colors { RED, BLACK };

struct inodes_tree_node {
    struct inode inode;
    __u32 id;
    __u32 count;
    enum tree_colors color;

    struct inodes_tree_node *parent;
    struct inodes_tree_node *left;
    struct inodes_tree_node *right;
};

struct inodes_tree {
    struct inodes_tree_node *root;
};

__u32 inodes_tree_put(struct inodes_tree *it, struct inode inode, __u32 id);
__u32 inodes_tree_unlink(struct inodes_tree *it, struct ext2_file_system *fs, __u32 id);