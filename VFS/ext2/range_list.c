#include "range_list.h"

__u32 range_list_init(struct range_list *list, __u32 start, __u32 end) {   
    list->head = malloc(sizeof(struct range_node));

    list->head->next = NULL;
    list->head->start = start;
    list->head->end = end;
    
    return 0;
}
__u32 range_list_destroy(struct range_list *list) {
    struct range_node *ptr = list->head;
    struct range_node *temp = NULL;

    while (ptr != NULL) {
        temp = ptr->next;
        free(ptr);
        ptr = temp;
    }

    return 0;
}

__u32 range_list_claim_number(struct range_list *list) {
    if (list->head == NULL) { return 0; }
    __u32 number = list->head->start;
    if (list->head->start == list->head->end) {
        struct range_node *temp = list->head->next;
        free(list->head);
        list->head = temp;
    } else {
        list->head->start++;
    }
    return number;
}

static struct range_node* merge(struct range_node *left, struct range_node *right) {
    if (left->end < right->start) { return NULL; }
    __u32 start = left->start;
    __u32 end = right->end;
    struct range_node *next = right->next;

    free(left);
    free(right);

    struct range_node *node = malloc(sizeof(struct range_node));
    node->next = next;
    node->start = start;
    node->end = end;
    return node;
}

/* absolute abomination. please review later */
__u32 range_list_unclaim_number(struct range_list *list, __u32 number) {
    if (list->head == NULL || list->head->start > number + 1) { 
        struct range_node *temp = list->head;
        list->head = malloc(sizeof(struct range_node));
        list->head->next = temp;
        list->head->start = list->head->end = number;
        return 0;
    }
    else if (list->head->start == number + 1) {
        list->head->start--;
        return 0;
    }
    struct range_node *left = list->head;
    struct range_node *right = NULL;
    if (left != NULL) { right = left->next; }
    while (right != NULL) {
        if (number + 1 == right->start) {
            right->start--;
            if (left->end + 1 == right->start) {
                struct range_node *temp = list->head;
                if (temp == left) { list->head = merge(left, right); return 0; }
                while (temp->next != left) { temp = temp->next; }
                temp->next = merge(left, right);
            }
            return 0;
        }
        else if (number - 1 == left->end) {
            left->end++;
            if (left->end + 1 == right->start) {
                struct range_node *temp = list->head;
                if (temp == left) { list->head = merge(left, right); return 0; }
                while (temp->next != left) { temp = temp->next; }
                temp->next = merge(left, right);
            }
            return 0;
        }
        else if (right->start > number + 1) {
            left->next = malloc(sizeof(struct range_node));
            left->next->next = right;
            left->next->start = left->next->end = number;
        }
        left = right;
        right = left->next;
    }
    if (number - 1 == left->end) {
        left->end++;
        return 0;
    }
    else {
        left->next = malloc(sizeof(struct range_node));
        left->next->next = NULL;
        left->next->start = left->next->end = number;
    }
    return 0;
}