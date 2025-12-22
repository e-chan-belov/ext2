#include "__u32_stack.h"

void __u32_stack_init(struct __u32_stack *s) {
    s->head = 0;
}
void __u32_stack_destroy(struct __u32_stack *s) {
    struct __u32_stack_node *temp;
    while (s->head != 0) {
        temp = s->head->parent;
        free(s->head);
        s->head = temp;
    }
}

void __u32_stack_push(struct __u32_stack *s, const __u32 value) {
    struct __u32_stack_node *temp = s->head;
    struct __u32_stack_node *node = malloc(sizeof(struct __u32_stack_node));
    node->value = value;
    node->parent = temp;
    s->head = node;
}
void __u32_stack_pop(struct __u32_stack *s) {
    if (s->head != 0) {
        struct __u32_stack_node *temp = s->head->parent;
        free(s->head);
        s->head = temp;
    }
}
__u32 __u32_stack_head(struct __u32_stack *s) {
    if (s->head != 0) {
        return s->head->value;
    }
    return 0;
}
__u8 __u32_stack_empty(struct __u32_stack *s) {
    return s->head == 0;
}