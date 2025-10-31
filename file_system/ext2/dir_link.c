#include "dir_link.h"

__u32 dir_link_init(struct dir_link *dl) {
    dl->head = 0;
}
__u32 dir_link_destroy(struct dir_link *dl) {
    struct dir_link_node *current = dl->head;
    struct dir_link_node *temp;
    while (current != 0) {
        temp = current->prev;
        free(current);
        current = temp;
    }
    return 0;
}

__u8 dir_link_is_id(struct dir_link *dl) {
    if (dl->head == 0) { return 0; }
    return dl->head->is_id;
}
__u32 dir_link_get_value(struct dir_link *dl) {
    if (dl->head == 0) { return 0; }
    return dl->head->value;
}

__u32 dir_link_add_value(struct dir_link *dl, __u32 value, __u8 is_id) {
    struct dir_link_node *node = malloc(sizeof(struct dir_link_node));
    node->value = value;
    node->is_id = is_id;
    node->prev = dl->head;

    dl->head = node;
    return 0;
}
__u32 dir_link_remove_value(struct dir_link *dl) {
    if (dl->head == 0) { return 1; }
    struct dir_link_node *temp = dl->head->prev;
    free(dl->head);
    dl->head = temp;
}