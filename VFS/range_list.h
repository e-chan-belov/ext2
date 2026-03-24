#pragma once
#include "types.h"
#include <stdlib.h>

struct range_node {
    struct range_node *next;

    /* start and end numbers are included like [start; end] */
    __u32 start;
    __u32 end;
};

struct range_list {
    struct range_node *head;
};

__u32 range_list_init(struct range_list *list, __u32 start, __u32 end);
__u32 range_list_destroy(struct range_list *list);

__u32 range_list_claim_number(struct range_list *list);
__u32 range_list_unclaim_number(struct range_list *list, __u32 number);