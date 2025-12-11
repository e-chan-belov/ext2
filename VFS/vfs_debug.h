#pragma once
#include "rb_tree.h"
#include <stdio.h>
#include <stdlib.h>

struct queue {
    int front, back, size;
    struct rb_node **data;
};

struct queue *queue_create(int capacity) {
    struct queue *q = (struct queue *)malloc(sizeof(struct queue));
    q->front = q->back = 0;
    q->size = capacity;
    q->data = (struct rb_node**)malloc(sizeof(struct rb_node*) * capacity);
    return q;
}

int queue_empty(struct queue *q) {
    return q->front == q->back;
}

void enqueue(struct queue *q, struct rb_node *n) {
    if (q->back < q->size)
        q->data[q->back++] = n;
}

struct rb_node* dequeue(struct queue *q) {
    if (queue_empty(q))
        return NULL;
    return q->data[q->front++];
}

void debug_red_black_tree(struct rb_tree *tree) {
    if (tree->root == NULL) {
        printf("(empty)\n");
        return;
    }

    struct queue *q = queue_create(1024);
    enqueue(q, tree->root);

    while (!queue_empty(q)) {
        int level_size = q->back - q->front;
        int i;
        for (i = 0; i < level_size; i++) {
            struct rb_node *n = dequeue(q);

            printf("%u(%c) ", n->key, n->color == RED ? 'R' : 'B');

            if (n->l) enqueue(q, n->l);
            if (n->r) enqueue(q, n->r);
        }

        printf("\n");
    }

    free(q->data);
    free(q);
}