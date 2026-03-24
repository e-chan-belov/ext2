#include "fd_table.h"

__u32 fd_table_init(struct fd_table *fd_table, struct inode_table *inode_table) {
    rb_tree_init(&fd_table->table);
    fd_table->inode_table = inode_table;
    fd_table->fs = inode_table->fs;
    return 0;
}

__u32 fd_table_destroy(struct fd_table *fd_table) {
    rb_tree_destroy(&fd_table->table);
    fd_table->fs = NULL;
    fd_table->inode_table = NULL;
    return 0;
}

static __u32 find_and_claim_fd_number(struct fd_table *fd_table) {
    // todo
}

static __u32 free_fd_number(struct fd_table *fd_table, __u32 fd) {
    return 0; // todo
}

__u32 fd_table_open(struct fd_table *fd_table, __u32 inode_id, __u32 flags) {
    __u32 fd_number = find_and_claim_fd_number(fd_table);

    struct inode *active_inode = inode_table_link_inode(fd_table->inode_table, inode_id);

    struct file_desc *fd = malloc(sizeof(struct file_desc));
    fd_init(fd, active_inode, inode_id, flags);
    rb_tree_insert(&fd_table->table, fd_number, fd);

    return fd_number;
}

__u32 fd_table_close(struct fd_table *fd_table, __u32 fd_id) {
    struct file_desc *fd = rb_tree_find(&fd_table->table, fd_id);

    inode_table_unlink_inode(fd_table->inode_table, fd->inode_id);

    fd_destroy(fd);
    rb_tree_delete(&fd_table->table, fd_id);
    
    free_fd_number(fd_table, fd_id);
    return 0;
}