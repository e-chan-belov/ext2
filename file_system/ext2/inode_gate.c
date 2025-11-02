#include "inode_gate.h"

__u32 inode_gate_init(struct inode_gate *ig, struct ext2_file_system *fs, struct inode *inode) {
    ig->fs = fs;
    ig->inode = inode;

    ig->lc = 0;

    ig->first_indirect_id = 0;
    ig->first_indirect_block = NULL;
    ig->second_indirect_id = 0;
    ig->second_indirect_block = NULL;
    ig->third_indirect_id = 0;
    ig->third_indirect_block = NULL;
    return 0;
}
__u32 inode_gate_destroy(struct inode_gate *ig) {
    if (ig->first_indirect_block != NULL) { block_munmap(ig->fs, ig->first_indirect_block); }
    if (ig->second_indirect_block != NULL) { block_munmap(ig->fs, ig->second_indirect_block); }
    if (ig->third_indirect_block != NULL) { block_munmap(ig->fs, ig->third_indirect_block); }
    ig->fs = NULL;
    ig->inode = NULL;
    return 0;
}

struct inode get_inode_copy(struct inode_gate *ig) {
    return *(ig->inode);
}

__u32 get_current_block_number(struct inode_gate *ig) {
    return ig->lc;
}


static __u32 does_lc_fit_in_file(struct inode_gate *ig, __u32 lc) {
    __u32 size_in_blocks = get_real_size_in_alloc_blocks(ig);
    if (lc >= 0 && lc < size_in_blocks) {
        return 1;
    }
    return 0;
}

__u32 next_block(struct inode_gate *ig) {
    if (does_lc_fit_in_file(ig, ig->lc + 1)) {
        ig->lc++;
        return 0;
    }
    return 1;
}
__u32 prev_block(struct inode_gate *ig) {
    if (ig->lc > 0) {
        ig->lc--;
        return 0;
    }
    return 1;
}
__u32 move_by_offset(struct inode_gate *ig, __s32 offset) {
    if (does_lc_fit_in_file(ig, ig->lc + offset)) {
        ig->lc++;
        return 0;
    }
    return 1;
}
__u32 move_to_first_block(struct inode_gate *ig) {
    ig->lc = 0;
    return 0;
}

static __u32 call_first_indirect(struct inode_gate *ig, __u32 block) {
    ig->first_indirect_id = block;
    ig->first_indirect_block = block_mmap(ig->fs, ig->first_indirect_id);
    return 0;
}
static __u32 check_and_cache_first_indirect(struct inode_gate *ig, __u32 hint) {
    if (hint == 0) { return 1; }
    if (ig->first_indirect_block == NULL) {
        call_first_indirect(ig, hint);
        return 0;
    }
    if (hint != ig->first_indirect_id) {
        block_munmap(ig->fs, ig->first_indirect_block);
        call_first_indirect(ig, hint);
        return 0;
    }
    return 0;
}

static __u32 call_second_indirect(struct inode_gate *ig, __u32 block) {
    ig->second_indirect_id = block;
    ig->second_indirect_block = block_mmap(ig->fs, ig->second_indirect_id);
    return 0;
}
static __u32 check_and_cache_second_indirect(struct inode_gate *ig, __u32 hint) {
    if (hint == 0) { return 1; }
    if (ig->second_indirect_block == NULL) {
        call_second_indirect(ig, hint);
        return 0;
    }
    if (hint != ig->second_indirect_id) {
        block_munmap(ig->fs, ig->second_indirect_block);
        call_second_indirect(ig, hint);
        return 0;
    }
    return 0;
}

static __u32 call_third_indirect(struct inode_gate *ig, __u32 block) {
    ig->third_indirect_id = block;
    ig->third_indirect_block = block_mmap(ig->fs, ig->third_indirect_id);
    return 0;
}
static __u32 check_and_cache_third_indirect(struct inode_gate *ig, __u32 hint) {
    if (hint == 0) { return 1; }
    if (ig->third_indirect_block == NULL) {
        call_third_indirect(ig, hint);
        return 0;
    }
    if (hint != ig->third_indirect_id) {
        block_munmap(ig->fs, ig->third_indirect_block);
        call_third_indirect(ig, hint);
        return 0;
    }
    return 0;
}

__u32 get_real_size_in_alloc_blocks(struct inode_gate *ig) {
    __u32 size = ig->inode->i_blocks * BLOCKS_SIZE_IN_I_BLOCKS / get_block_size_from_fs(ig->fs);
    if (ig->inode->i_block[FIRST_INDIRECT_BLOCK_ID] == 0) { return size; }
    size -= 1;
    if (ig->inode->i_block[SECOND_INDIRECT_BLOCK_ID] == 0) { return size; }
    size -= 1;
    check_and_cache_second_indirect(ig, ig->inode->i_block[SECOND_INDIRECT_BLOCK_ID]);
    __u32 i;
    for (i = 0; ig->second_indirect_block[i] != 0 && i < get_block_size_from_fs(ig->fs) / sizeof(__u32); i++) {
        size -= 1;
    }
    if (ig->inode->i_block[THIRD_INDIRECT_BLOCK_ID] == 0) { return size; }
    size -= 1;
    check_and_cache_third_indirect(ig, ig->inode->i_block[THIRD_INDIRECT_BLOCK_ID]);
    __u32 j;
    for (i = 0; ig->third_indirect_block[i] != 0 && i < get_block_size_from_fs(ig->fs) / sizeof(__u32); i++) {
        check_and_cache_second_indirect(ig, ig->third_indirect_block[i]);
        for (j = 0; ig->second_indirect_block[j] != 0 && j < get_block_size_from_fs(ig->fs) / sizeof(__u32); j++) {
            size -= 1;
        }
    }
    return size;
}

__u32 get_current_block_id(struct inode_gate *ig) {
    if (ig->lc < DIRECT_INDEXES_AMOUNT) {
        return ig->inode->i_block[ig->lc];
    }

    __u32 pointers_amount_in_block = get_block_size_from_fs(ig->fs) / sizeof(__u32);
    __u32 cur_lc = ig->lc - DIRECT_INDEXES_AMOUNT;

    if (cur_lc < pointers_amount_in_block) {
        check_and_cache_first_indirect(ig, ig->inode->i_block[FIRST_INDIRECT_BLOCK_ID]);
        return ig->first_indirect_block[cur_lc];
    }

    cur_lc -= pointers_amount_in_block;

    if (cur_lc < pointers_amount_in_block * pointers_amount_in_block) {
        check_and_cache_second_indirect(ig, ig->inode->i_block[SECOND_INDIRECT_BLOCK_ID]);
        check_and_cache_first_indirect(ig, ig->second_indirect_block[cur_lc / pointers_amount_in_block]);
        return ig->first_indirect_block[cur_lc % pointers_amount_in_block];
    }

    cur_lc -= pointers_amount_in_block * pointers_amount_in_block;

    if (cur_lc < pointers_amount_in_block * pointers_amount_in_block * pointers_amount_in_block) {
        check_and_cache_third_indirect(ig, ig->inode->i_block[THIRD_INDIRECT_BLOCK_ID]);
        check_and_cache_second_indirect(ig, ig->third_indirect_block[cur_lc / (pointers_amount_in_block * pointers_amount_in_block)]);
        check_and_cache_first_indirect(ig, ig->second_indirect_block[cur_lc / pointers_amount_in_block % pointers_amount_in_block]);
        return ig->first_indirect_block[cur_lc % pointers_amount_in_block];
    }
    return 0;
}

__u32 change_inode_size(struct inode_gate *ig, __s32 offset) {
    ig->inode->i_size += offset;
    return 0;
}

static __u32 alloc_block_for_indirection(struct inode_gate *ig, __u32 hint) {
    ig->inode->i_blocks += get_block_size_from_fs(ig->fs) / BLOCKS_SIZE_IN_I_BLOCKS;
    return block_alloc(ig->fs, first_free_block(ig->fs, hint));
}

__u32 append_block(struct inode_gate *ig, __u32 block) {
    __u32 current_size = get_real_size_in_alloc_blocks(ig);
    ig->inode->i_blocks += get_block_size_from_fs(ig->fs) / BLOCKS_SIZE_IN_I_BLOCKS;
    if (current_size < DIRECT_INDEXES_AMOUNT) {
        ig->inode->i_block[current_size] = block;
        return 0;
    }
    __u32 last_lc = current_size;
    last_lc -= DIRECT_INDEXES_AMOUNT;

    __u32 pointers_amount_in_block = get_block_size_from_fs(ig->fs) / sizeof(__u32);
    if (last_lc == 0) {
        ig->inode->i_block[FIRST_INDIRECT_BLOCK_ID] = alloc_block_for_indirection(ig, block + 1);
    }
    if (last_lc < pointers_amount_in_block) {
        check_and_cache_first_indirect(ig, ig->inode->i_block[FIRST_INDIRECT_BLOCK_ID]);
        ig->first_indirect_block[last_lc] = block;
        return 0;
    }
    last_lc -= pointers_amount_in_block;

    if (last_lc == 0) {
        ig->inode->i_block[SECOND_INDIRECT_BLOCK_ID] = alloc_block_for_indirection(ig, block + 1);
    }
    if (last_lc % pointers_amount_in_block == 0 && last_lc < pointers_amount_in_block * pointers_amount_in_block) {
        check_and_cache_second_indirect(ig, ig->inode->i_block[SECOND_INDIRECT_BLOCK_ID]);
        ig->second_indirect_block[last_lc / pointers_amount_in_block] = alloc_block_for_indirection(ig, block + 1);
    }
    if (last_lc < pointers_amount_in_block * pointers_amount_in_block) {
        check_and_cache_second_indirect(ig, ig->inode->i_block[SECOND_INDIRECT_BLOCK_ID]);
        check_and_cache_first_indirect(ig, ig->second_indirect_block[last_lc / pointers_amount_in_block]);
        ig->first_indirect_block[last_lc % pointers_amount_in_block] = block;
        return 0;
    }
    last_lc -= pointers_amount_in_block * pointers_amount_in_block;

    if (last_lc == 0) {
        ig->inode->i_block[THIRD_INDIRECT_BLOCK_ID] = alloc_block_for_indirection(ig, block + 1);
    }
    if (last_lc % (pointers_amount_in_block * pointers_amount_in_block) == 0) {
        check_and_cache_third_indirect(ig, ig->inode->i_block[THIRD_INDIRECT_BLOCK_ID]);
        ig->third_indirect_block[last_lc / (pointers_amount_in_block * pointers_amount_in_block)] = alloc_block_for_indirection(ig, block + 1);
    }
    if (last_lc % pointers_amount_in_block == 0) {
        check_and_cache_third_indirect(ig, ig->inode->i_block[THIRD_INDIRECT_BLOCK_ID]);
        check_and_cache_second_indirect(ig, ig->third_indirect_block[last_lc / (pointers_amount_in_block * pointers_amount_in_block)]);
        ig->second_indirect_block[last_lc / pointers_amount_in_block] = alloc_block_for_indirection(ig, block + 1);
    }
    if (last_lc < pointers_amount_in_block * pointers_amount_in_block * pointers_amount_in_block) {
        check_and_cache_third_indirect(ig, ig->inode->i_block[THIRD_INDIRECT_BLOCK_ID]);
        check_and_cache_second_indirect(ig, ig->third_indirect_block[last_lc / (pointers_amount_in_block * pointers_amount_in_block)]);
        check_and_cache_first_indirect(ig, ig->second_indirect_block[last_lc / pointers_amount_in_block % pointers_amount_in_block]);
        ig->first_indirect_block[last_lc % pointers_amount_in_block] = block;
        return 0;
    }
    return 1;
}

static __u32 free_indirection_block(struct inode_gate *ig, __u32 block) {
    ig->inode->i_blocks -= get_block_size_from_fs(ig->fs) / BLOCKS_SIZE_IN_I_BLOCKS;
    return free_block(ig->fs, block);
}

// retuns an id of the unlinked block
__u32 unlink_last_block(struct inode_gate *ig) {
    __u32 current_size = get_real_size_in_alloc_blocks(ig);
    if (current_size == 0) { return 0; }
    ig->inode->i_blocks -= get_block_size_from_fs(ig->fs) / BLOCKS_SIZE_IN_I_BLOCKS;
    if (current_size < DIRECT_INDEXES_AMOUNT) {
        __u32 temp = ig->inode->i_block[current_size - 1];
        ig->inode->i_block[current_size - 1] = 0;
        return temp;
    }

    __u32 ret_id;
    __u32 last_lc = current_size - 1;
    last_lc -= DIRECT_INDEXES_AMOUNT;
    __u32 pointers_amount_in_block = get_block_size_from_fs(ig->fs) / sizeof(__u32);

    if (last_lc < pointers_amount_in_block) {
        check_and_cache_first_indirect(ig, ig->inode->i_block[FIRST_INDIRECT_BLOCK_ID]);
        
        ret_id = ig->first_indirect_block[last_lc];
        free_block(ig->fs, ig->first_indirect_block[last_lc]);
        ig->first_indirect_block[last_lc] = 0;
    }
    if (last_lc == 0) {
        free_indirection_block(ig, ig->inode->i_block[FIRST_INDIRECT_BLOCK_ID]);
        ig->inode->i_block[FIRST_INDIRECT_BLOCK_ID] = 0;
    }

    last_lc -= pointers_amount_in_block;
    if (last_lc < pointers_amount_in_block * pointers_amount_in_block) {
        check_and_cache_second_indirect(ig, ig->inode->i_block[SECOND_INDIRECT_BLOCK_ID]);
        check_and_cache_first_indirect(ig, ig->second_indirect_block[last_lc / pointers_amount_in_block]);
        
        ret_id = ig->first_indirect_block[last_lc % pointers_amount_in_block];
        free_block(ig->fs, ig->first_indirect_block[last_lc % pointers_amount_in_block]);
        ig->first_indirect_block[last_lc % pointers_amount_in_block] = 0;
    }
    if (last_lc % pointers_amount_in_block == 0 && last_lc < pointers_amount_in_block * pointers_amount_in_block) {
        check_and_cache_second_indirect(ig, ig->inode->i_block[SECOND_INDIRECT_BLOCK_ID]);
        free_indirection_block(ig, ig->second_indirect_block[last_lc / pointers_amount_in_block]);
        ig->second_indirect_block[last_lc / pointers_amount_in_block] = 0;
    }
    if (last_lc == 0) {
        free_indirection_block(ig, ig->inode->i_block[SECOND_INDIRECT_BLOCK_ID]);
        ig->inode->i_block[SECOND_INDIRECT_BLOCK_ID] = 0;
    }

    last_lc -= pointers_amount_in_block * pointers_amount_in_block;
    if (last_lc < pointers_amount_in_block * pointers_amount_in_block * pointers_amount_in_block) {
        check_and_cache_third_indirect(ig, ig->inode->i_block[THIRD_INDIRECT_BLOCK_ID]);
        check_and_cache_second_indirect(ig, ig->third_indirect_block[last_lc / (pointers_amount_in_block * pointers_amount_in_block)]);
        check_and_cache_first_indirect(ig, ig->second_indirect_block[last_lc / pointers_amount_in_block % pointers_amount_in_block]);
        
        ret_id = ig->first_indirect_block[last_lc % pointers_amount_in_block];
        free_block(ig->fs, ig->first_indirect_block[last_lc % pointers_amount_in_block]);
        ig->first_indirect_block[last_lc % pointers_amount_in_block] = 0;
    }
    if (last_lc % pointers_amount_in_block == 0) {
        check_and_cache_third_indirect(ig, ig->inode->i_block[THIRD_INDIRECT_BLOCK_ID]);
        check_and_cache_second_indirect(ig, ig->third_indirect_block[last_lc / (pointers_amount_in_block * pointers_amount_in_block)]);
        free_indirection_block(ig, ig->second_indirect_block[last_lc / pointers_amount_in_block]);
        ig->second_indirect_block[last_lc / pointers_amount_in_block] = 0;
    }
    if (last_lc % (pointers_amount_in_block * pointers_amount_in_block) == 0) {
        check_and_cache_third_indirect(ig, ig->inode->i_block[THIRD_INDIRECT_BLOCK_ID]);
        free_indirection_block(ig, ig->third_indirect_block[last_lc / (pointers_amount_in_block * pointers_amount_in_block)]);
        ig->third_indirect_block[last_lc / (pointers_amount_in_block * pointers_amount_in_block)] = 0;
    }
    if (last_lc == 0) {
        free_indirection_block(ig, ig->inode->i_block[THIRD_INDIRECT_BLOCK_ID]);
        ig->inode->i_block[THIRD_INDIRECT_BLOCK_ID] = 0;
    }
    return ret_id;
}