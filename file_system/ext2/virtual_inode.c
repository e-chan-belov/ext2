#include "virtual_inode.h"

void virtual_inode_init(struct virtual_inode *vi, struct ext2_file_system *fs, struct inode *inode) {
    vi->fs = fs;
    vi->inode = inode;

    vi->direct_index = 0;
    vi->first_index = 0;
    vi->second_index = 0;
    vi->third_index = 0;

    vi->first_block = NULL;
    vi->second_block = NULL;
    vi->third_block = NULL;
}

void virtual_inode_destroy(struct virtual_inode *vi) {
    if (vi->first_block != NULL) { block_munmap(vi->fs, vi->first_block); }
    if (vi->second_block != NULL) { block_munmap(vi->fs, vi->second_block); }
    if (vi->third_block != NULL) { block_munmap(vi->fs, vi->third_block); }
}

/* this is garbage. please fix spaghetti-code.
** there is a lot of repeating chunks of code.
** try using logic_index instead of constant checks
*/
void next_block(struct virtual_inode *vi) {
    if (vi->direct_index < 11) {
        vi->direct_index++;
        return;
    }

    /* TODO: check_and_restore_cached_id(vi) for race condition */

    if (vi->cached_first_id == 0) {
        vi->direct_index = 12;
        vi->cached_first_id = vi->inode->i_block[vi->direct_index];
        vi->first_block = block_mmap(vi->fs, vi->cached_first_id);
        return;
    }
    if (vi->first_index < vi->fs->block_size / sizeof(__u32)) {
        vi->first_index++;
        return;
    }

    vi->first_index = 0;
    block_munmap(vi->fs, vi->first_block);

    if (vi->cached_second_id == 0) {
        vi->direct_index = 13;

        vi->cached_second_id = vi->inode->i_block[vi->direct_index];
        vi->second_block = block_mmap(vi->fs, vi->cached_second_id);
        vi->cached_first_id = vi->second_block[0];
        vi->first_block = block_mmap(vi->fs, vi->cached_first_id);
        return;
    }
    if (vi->second_index < vi->fs->block_size / sizeof(__u32)) {
        vi->second_index++;

        vi->cached_first_id = vi->second_block[vi->second_index];
        vi->first_block = block_mmap(vi->fs, vi->cached_first_id);
        return;
    }

    vi->second_index = 0;
    block_munmap(vi->fs, vi->second_block);
    
    if (vi->cached_third_id == 0) {
        vi->direct_index = 14;

        vi->cached_third_id = vi->inode->i_block[vi->direct_index];
        vi->third_block = block_mmap(vi->fs, vi->cached_third_id);
        vi->cached_second_id = vi->third_block[0];
        vi->second_block = block_mmap(vi->fs, vi->cached_second_id);
        vi->cached_first_id = vi->second_block[0];
        vi->first_block = block_mmap(vi->fs, vi->cached_first_id);
        return;
    }
    if (vi->third_index < vi->fs->block_size / sizeof(__u32)) {
        vi->third_index++;

        vi->cached_second_id = vi->third_block[vi->third_index];
        vi->second_block = block_mmap(vi->fs, vi->cached_second_id);
        vi->cached_first_id = vi->second_block[0];
        vi->first_block = block_mmap(vi->fs, vi->cached_first_id);
        return;
    }
    printf("Out of bounce!\n");
}

__u32 is_current_block_in_file(struct virtual_inode *vi) {
    if (vi->direct_index < 12) {
        return !!vi->inode->i_block[vi->direct_index];
    }
    return !!vi->first_block[vi->first_index];
}

__u32 get_block_id(struct virtual_inode *vi) {
    if (vi->direct_index < 12) {
        return vi->inode->i_block[vi->direct_index];
    }
    return vi->first_block[vi->first_index];
}