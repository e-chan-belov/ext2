#include "rb_tree.h"

struct rb_node* rb_node_create(__u32 key_, void *value_, enum tree_colors color_, struct rb_node *l_, struct rb_node *r_) {
    struct rb_node *n = malloc(sizeof(struct rb_node));
    n->key = key_;
    n->value = value_;
    n->color = color_;
    n->l = l_;
    n->r = r_;
    return n;
}
__u32 rb_node_and_value_free(struct rb_node* n) {
    if (n == NULL) { return 1; }
    if (n->value != NULL) { free(n->value); } // strange decision
    free(n);
    return 0;
}

__u32 rb_tree_init(struct rb_tree *tree) {
    tree->root = NULL;
    return 0;
}

struct rb_node* rb_tree_find(struct rb_tree *tree, __u32 key) {
    struct rb_node* p = tree->root;
    while (p != NULL) {
        if (p->key == key) { return p; }
        if (p->key < key) {
            p = p->r;
        }
        else {
            p = p->l;
        }
    }
    return NULL;
}

static __u32 left_rotate(struct rb_tree *tree, struct rb_node *n) {
    if (tree == NULL || n == NULL) { return 1; }
    struct rb_node *p = n->r;

    p->p = n->p;
    if (n->p == NULL) {
        tree->root = p;
    }
    else {
        if (n->p->l == n) {
            n->p->l = p;
        }
        else {
            n->p->r = p;
        }
    }

    n->r = p->l;
    if (p->l != NULL) {
        p->l->p = n;
    }

    n->p = p;
    p->l = n;
    return 0;
}
static __u32 right_rotate(struct rb_tree *tree, struct rb_node *n) {
    if (tree == NULL || n == NULL) { return 1; }
    struct rb_node *p = n->l;

    p->p = n->p;
    if (n->p == NULL) {
        tree->root = p;
    }
    else {
        if (n->p->l == n) {
            n->p->l = p;
        }
        else {
            n->p->r = p;
        }
    }

    n->l = p->r;
    if (p->r != NULL) {
        p->r->p = n;
    }

    n->p = p;
    p->r = n;
    return 0;
}


static __u32 fix_insertion(struct rb_tree *tree, struct rb_node *n) {
    if (tree == NULL || n == NULL) { return 1; }
    if (n == tree->root) {
        n->color = BLACK;
        return 0;
    }
    while (n->p != NULL && n->p->color == RED) {
        if (n->p == n->p->p->l) { // father is a left child
            if (n->p->p->r != NULL && n->p->p->r->color == RED) { // there is a red uncle
                n->p->color = BLACK;
                n->p->p->r->color = BLACK;
                n->p->p->color = RED;
                n = n->p->p;
            }
            else {
                if (n == n->p->r) {
                    n = n->p;
                    left_rotate(tree, n);
                }
                n->p->color = BLACK;
                n->p->p->color = RED;
                right_rotate(tree, n->p->p);
            }
        }
        else {
            if (n->p->p->l != NULL && n->p->p->l->color == RED) {
                n->p->color = BLACK;
                n->p->p->l->color = BLACK;
                n->p->p->color = RED;
                n = n->p->p;
            }
            else {
                if (n == n->p->l) {
                    n = n->p;
                    right_rotate(tree, n);
                }
                n->p->color = BLACK;
                n->p->p->color = RED;
                left_rotate(tree, n->p->p);
            }
        }
    }
    tree->root->color = BLACK;
    return 0;
}

__u32 rb_tree_insert(struct rb_tree *tree, __u32 key, void *value) {
    struct rb_node *n = rb_node_create(key, value, RED, NULL, NULL);
    if (tree->root == NULL) {
        tree->root = n;
        n->p = NULL;
    }
    else {
        struct rb_node *p = tree->root;
        struct rb_node *q = NULL;
        while (p != NULL) {
            q = p;
            if (p->key < n->key) {
                p = p->r;
            }
            else {
                p = p->l;
            }
            n->p = q;
        }
        if (q->key < n->key) {
            q->r = n;
        }
        else {
            q->l = n;
        }
    }
    fix_insertion(tree, n);
}

static __u32 transplant(struct rb_tree *tree, struct rb_node *n, struct rb_node *v) {
    if (tree == NULL) { return 1; }

    if (n->p == NULL) {
        tree->root = v;
    }
    else if (n == n->p->l) {
        n->p->l = v;
    }
    else {
        n->p->r = v;
    }
    if (v != NULL) {
        v->p = n->p;
    }
    
    return 0;
}
static __u32 minimum(struct rb_node *root) {
    if (root == NULL) { return 0; }

    struct rb_node *p = root;
    while (p->l != NULL) {
        p = p->l;
    }
    return p->key;
}

__u32 rb_tree_delete(struct rb_tree *tree, __u32 key){
    if (tree == NULL) { return 1; }

    struct rb_node *z = rb_tree_find(tree, key);
    if (z->l == NULL) {
        transplant(tree, z, z->r);
        rb_node_and_value_free(z);
    }
    else if (z->r == NULL) {
        transplant(tree, z, z->l);
        rb_node_and_value_free(z);
    }
    else {
        struct rb_node *y = rb_tree_find(tree, minimum(z->r));
        if (y->p != z) {
            transplant(tree, y, y->r);
            y->r = z->r;
            y->r->p = y;
        }
        transplant(tree, z, y);
        y->l = z->l;
        y->l->p = y;
        rb_node_and_value_free(z);
    }
    return 0;
}