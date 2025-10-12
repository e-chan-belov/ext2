#include "super_block.h"

__u32 get_block_size(struct super_block *sb) {
    return 1024 << sb->s_log_block_size;
}

__u32 get_frag_size(struct super_block *sb) {
    if (sb->s_log_frag_size > 0) {
        return 1024 << sb->s_log_frag_size;
    }
    else {
        return 1024 >> -sb->s_log_frag_size;
    }
}