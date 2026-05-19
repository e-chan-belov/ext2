#include "fd_table.h"

__u32 fd_table_init(struct fd_table *fd_table, struct inode_table *inode_table) {
    rb_tree_init(&fd_table->table);
    range_list_init(&fd_table->number_list, FIRST_POTENTIAL_FD_NUMBER, MAX_AMOUNT_OF_FD - 1);
    fd_table->inode_table = inode_table;
    fd_table->fs = inode_table->fs;
    return 0;
}

__u32 fd_table_destroy(struct fd_table *fd_table) {
    rb_tree_destroy(&fd_table->table);
    range_list_destroy(&fd_table->number_list);
    fd_table->fs = NULL;
    fd_table->inode_table = NULL;
    return 0;
}

static __u32 find_and_claim_fd_number(struct fd_table *fd_table) {
    return range_list_claim_number(&fd_table->number_list);
}

static __u32 free_fd_number(struct fd_table *fd_table, __u32 fd) {
    range_list_unclaim_number(&fd_table->number_list, fd);
    return 0;
}

__u32 fd_table_open(struct fd_table *fd_table, struct ext2_file_system *fs, __u32 inode_id, __u32 flags) {
    __u32 fd_number = find_and_claim_fd_number(fd_table);
    if (fd_number < FIRST_POTENTIAL_FD_NUMBER) { // usually it happens when the number is zero
        return 0;
    }

    struct inode *active_inode = inode_table_link_inode(fd_table->inode_table, inode_id);

    struct file_desc *fd = malloc(sizeof(struct file_desc));
    fd_init(fd, fs, active_inode, inode_id, flags);
    rb_tree_insert(&fd_table->table, fd_number, fd);

    return fd_number;
}

__u32 fd_table_close(struct fd_table *fd_table, __u32 fd_id) {
    struct file_desc *fd = rb_tree_find(&fd_table->table, fd_id)->value;

    inode_table_unlink_inode(fd_table->inode_table, fd->inode_id);

    fd_destroy(fd);
    rb_tree_delete(&fd_table->table, fd_id);
    
    free_fd_number(fd_table, fd_id);
    return 0;
}

__u32 fd_table_move_by_offset(struct fd_table *fd_table, __u32 fd_id, __u32 offset) {
    struct file_desc *fd = rb_tree_find(&fd_table->table, fd_id)->value;

    return fd_move_by_offset(fd, offset);
}

__u32 fd_table_read(struct fd_table *fd_table,__u32 fd_id, __u32 size, void *buf) {
    struct file_desc *fd = rb_tree_find(&fd_table->table, fd_id)->value;

    return fd_read(fd, size, buf);
}
__u32 fd_table_write(struct fd_table *fd_table,__u32 fd_id, __u32 size, void *buf) {
    struct file_desc *fd = rb_tree_find(&fd_table->table, fd_id)->value;

    return fd_write(fd, size, buf);
}

__u32 fd_table_ftruncate(struct fd_table *fd_table,__u32 fd_id, __u32 length) {
    struct file_desc *fd = rb_tree_find(&fd_table->table, fd_id)->value;

    return fd_ftruncate(fd, length);
}