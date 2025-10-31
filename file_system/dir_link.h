#pragma once

#include "types.h"

// value is either offset or inode id
struct dir_link_node {
    __u32 value;
    __u8 is_id;
    struct dir_link_node *prev;
};

struct dir_link {
    struct dir_link_node *head;
};

__u32 dir_link_init(struct dir_link *dl);
__u32 dir_link_destroy(struct dir_link *dl);

__u8 dir_link_is_id(struct dir_link *dl);
__u32 dir_link_get_value(struct dir_link *dl);

__u32 dir_link_add_value(struct dir_link *dl, __u32 value, __u8 is_id);
__u32 dir_link_remove_value(struct dir_link *dl);
