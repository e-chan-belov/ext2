#pragma once
/*
#include <string.h>
#include "inode.h"
#include "ext2_fs.h"

struct file {
    struct ext2_file_system *fs;
    struct inode inode;
    void* ptr;
};

int file_init(struct file *f, struct ext2_file_system *fs, __u32 id) {
    f->fs = fs;
    f->inode = read_inode(f->fs, id);
    f->ptr = NULL;
    return 0;
}

int read_open_file(struct file *f) {
    if (f->inode.i_size == 0) { return 0; }
    f->ptr = malloc(f->inode.i_size);
    __u32 address_count = f->fs->block_size / sizeof(__u32);
    int i;
    __u32 block_id;
    void *block_ptr;
    for (i = 0; i < 12; i++) {
        block_id = f->inode.i_block[i];
        if (block_id == 0) { return 0; }
        block_ptr = block_alloc(f->fs, block_id);
        memcpy(f->ptr + i * f->fs->block_size, block_ptr, f->fs->block_size);
        free_block(f->fs, block_ptr);
    }

    if (f->inode.i_block[12] == 0) { return 0; }
    __u32 *first_indirect_block = block_alloc(f->fs, f->inode.i_block[12]);
    for (i = 0; i < address_count; i++) {
        block_id = first_indirect_block[i];
        if (block_id == 0) { free_block(f->fs, first_indirect_block); return 0; }
        block_ptr = block_alloc(f->fs, block_id);
        memcpy(f->ptr + (12 + i) * f->fs->block_size, block_ptr, f->fs->block_size);
        free_block(f->fs, block_ptr);
    }
    free_block(f->fs, first_indirect_block);

    if (f->inode.i_block[13] == 0) { return 0; }
    __u32 *second_indirect_block = block_alloc(f->fs, f->inode.i_block[13]);
    
    int j;
    for (i = 0; i < address_count; i++) {
        if (second_indirect_block[i] == 0) { free_block(f->fs, second_indirect_block); return 0; }
        first_indirect_block = block_alloc(f->fs, second_indirect_block[i]);
        for (j = 0; j < address_count; j++) {
            block_id = first_indirect_block[j];
            if (block_id == 0) {
                free_block(f->fs, first_indirect_block);
                free_block(f->fs, second_indirect_block);
                return 0;
            }
            block_ptr = block_alloc(f->fs, block_id);
            memcpy(f->ptr + (12 + address_count + i) * f->fs->block_size, block_ptr, f->fs->block_size); // todo
            free_block(f->fs, block_ptr);
        }
        free_block(f->fs, first_indirect_block);
    }
    free_block(f->fs, second_indirect_block);

    if (f->inode.i_block[14] == 0) { return 0; }
    __u32 *third_indirect_block = block_alloc(f->fs, f->inode.i_block[14]);
    int k;
    for (i = 0; i < address_count; i++) {
        if (third_indirect_block[i] == 0) { free_block(f->fs, third_indirect_block); return 0;}
        second_indirect_block = block_alloc(f->fs, third_indirect_block[i]);
        for (j = 0; j < address_count; j++) {
            if (second_indirect_block[j] == 0) {
                free_block(f->fs, second_indirect_block);
                free_block(f->fs, third_indirect_block);
                return 0;
            }
            first_indirect_block = block_alloc(f->fs, second_indirect_block[j]);
            for (k = 0; k < address_count; k++) {
                block_id = first_indirect_block[k];
                if (block_id == 0) {
                    free_block(f->fs, first_indirect_block);
                    free_block(f->fs, second_indirect_block);
                    free_block(f->fs, third_indirect_block);
                    return 0;
                }
                block_ptr = block_alloc(f->fs, block_id);
                memcpy(f->ptr + (12 + address_count + address_count * address_count + i) * f->fs->block_size, block_ptr, f->fs->block_size); // todo
                free_block(f->fs, block_ptr);
            }
            free_block(f->fs, first_indirect_block);
        }
        free_block(f->fs, second_indirect_block);
    }
    free_block(f->fs, third_indirect_block);
    return 0;
}

void close_file(struct file *f) {
    free(f->ptr);
}*/