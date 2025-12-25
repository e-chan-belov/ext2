#include "inode_table.h"

__u32 inode_table_entry_init(struct inode_table_entry *ite, struct inode inode) {
    ite->inode = inode;
    ite->links_count = 0;
    return 0;
}

__u32 inode_table_entry_add_link(struct inode_table_entry *ite) {
    return ++ite->links_count;
}

__u32 inode_table_entry_remove_link(struct inode_table_entry *ite) {
    return --ite->links_count;
}

__u8 inode_table_entry_are_links_empty(struct inode_table_entry *ite) {
    return ite->links_count == 0;
}

__u32 inode_table_init(struct inode_table* it, struct ext2_file_system *fs) {
    rb_tree_init(&(it->table));
    it->fs = fs;
}

__u32 inode_table_destroy(struct inode_table* it) {
    rb_tree_destroy(&(it->table));
    it->fs = NULL;
}

struct inode* inode_table_link_inode(struct inode_table *it, __u32 id) {
    struct rb_node *n = rb_tree_find(&(it->table), id);
    if (n == NULL) {
        struct inode inode = read_inode(it->fs, id);

        struct inode_table_entry *ite = malloc(sizeof(struct inode_table_entry));
        inode_table_entry_init(ite, inode);
        inode_table_entry_add_link(ite);

        rb_tree_insert(&(it->table), id, ite);
        return &(ite->inode);
    }
    inode_table_entry_add_link((struct inode_table_entry*)(n->value));
    return &(((struct inode_table_entry*)(n->value))->inode);
}

__u32 inode_table_unlink_inode(struct inode_table *it, __u32 id) {
    struct rb_node *n = rb_tree_find(&it->table, id);
    if (n == NULL) { return 1; }
    struct inode_table_entry *ite = (struct inode_table_entry*)(n->value);

    inode_table_entry_remove_link(ite);
    if (inode_table_entry_are_links_empty(ite)) {
        put_inode(it->fs, ite->inode, id);
        rb_tree_delete(&it->table, id);
    }
    return 0;
}