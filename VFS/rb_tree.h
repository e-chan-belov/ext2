#pragma once
#include "types.h"
#include <stdlib.h>

struct rb_node {
    struct rb_node *p;
    struct rb_node *l;
    struct rb_node *r;
    
    enum tree_colors color;
    __u32 key;

    void *value;
};

struct rb_node* rb_node_create(__u32 key_, void *value_, enum tree_colors color_, struct rb_node *l_, struct rb_node *r_);
__u32 rb_node_and_value_free(struct rb_node* n);

struct rb_tree {
    struct rb_node *root;
};

__u32 rb_tree_init(struct rb_tree *tree);
__u32 rb_tree_destroy(struct rb_tree *tree);

struct rb_node* rb_tree_find(struct rb_tree *tree, __u32 key);
__u32 rb_tree_insert(struct rb_tree *tree, __u32 key, void *value);
__u32 rb_tree_delete(struct rb_tree *tree, __u32 key);