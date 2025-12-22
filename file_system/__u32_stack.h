#pragma once

#include "types.h"
#include "stdlib.h"

struct __u32_stack_node {
    __u32 value;
    struct __u32_stack_node *parent;
};

struct __u32_stack {
    struct __u32_stack_node *head;
};

void __u32_stack_init(struct __u32_stack *s);
void __u32_stack_destroy(struct __u32_stack *s);

void __u32_stack_push(struct __u32_stack *s, const __u32 value);
void __u32_stack_pop(struct __u32_stack *s);
__u32 __u32_stack_head(struct __u32_stack *s);
__u8 __u32_stack_empty(struct __u32_stack *s);