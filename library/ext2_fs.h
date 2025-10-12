#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/mman.h>

#include "types.h"
#include "super_block.h"
#include "bgdt.h"
#include "inode.h"


struct ext2_file_system {
    int fd;
    struct super_block sb;
    __u32 block_size;
    struct block_group_descriptor_table* bgdt;
    __u32 groups_count;
};

int ext2_file_system_init(struct ext2_file_system *me, const char *file) {
    int fd = open(file, O_RDWR);

    int err;
    void *ptr = mmap(NULL, 2048, PROT_READ, MAP_PRIVATE, fd, 0);
    if (ptr == MAP_FAILED) {
        return -2;
    }

    me->fd = fd;
    me->sb = *(struct super_block*)(ptr + 1024);

    err = munmap(ptr, 1024);
    if (err < 0) {
        return -3;
    }

    me->block_size = get_block_size(&me->sb);
    me->groups_count = CEIL_DIV(me->sb.s_blocks_count, me->sb.s_blocks_per_group);

    int sz = me->groups_count * sizeof(struct block_group_descriptor_table);
    me->bgdt = malloc(sz);
    ptr = mmap(NULL, sz, PROT_READ, MAP_PRIVATE, fd, me->block_size); // THIS MAY CAUSE MAP_FAILED!!!!!
    if (ptr == MAP_FAILED) {
        return -4;
    }

    struct block_group_descriptor_table*tmp_ptr = ptr;
    int i;
    for (i = 0; i < me->groups_count; i++, tmp_ptr++) {
        me->bgdt[i] = *tmp_ptr;
    }

    err = munmap(ptr, sz);
    if (err < 0) {
        return -5;
    }
    return 0;
}

void ext2_file_system_destroy(struct ext2_file_system *me) {
    free(me->bgdt);
    close(me->fd);
}

// VERY UNSAFE CODE !!!!!!!!!!!!!!!!
void* block_alloc(struct ext2_file_system *me, __u32 id) {
    void *ptr = mmap(NULL, me->block_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, me->fd, me->block_size * id);
    return ptr;
}

void* block_malloc(struct ext2_file_system *me, __u32 first_id, __u32 count) {
    void *ptr = mmap(NULL, me->block_size * count, PROT_READ | PROT_WRITE, MAP_PRIVATE, me->fd, me->block_size * first_id);
}

void free_block(struct ext2_file_system *me, void *ptr) {
    munmap(ptr, me->block_size);
}

void free_blocks(struct ext2_file_system *me, void *ptr, __u32 count) {
    munmap(ptr, me->block_size * count);
}

// returns a copy of the requested inode structure VERY UNSAFE CODE !!!!!!!!!!!!!!!!!!!
struct inode read_inode(struct ext2_file_system *me, __u32 inode) {
    __u32 block_group_index = (inode - 1) / me->sb.s_inodes_per_group;
    __u32 local_inode_index = (inode - 1) % me->sb.s_inodes_per_group;
    __u32 id = me->bgdt[block_group_index].bg_inode_table + local_inode_index / (me->block_size / me->sb.s_inode_size);
    void *ptr = block_alloc(me, id);
    struct inode temp = *(struct inode*)(ptr + me->sb.s_inode_size * (local_inode_index % (me->block_size / me->sb.s_inode_size)));
    free_block(me, ptr);
    return temp;
}

__u8 is_block_used(struct ext2_file_system *fs, __u32 id) {
    __u32 group = id / fs->sb.s_blocks_per_group;
    id = id % fs->sb.s_blocks_per_group;
    void *bitmap = block_alloc(fs, fs->bgdt[group].bg_block_bitmap);
    __u8 ans = !!(*(__u8*)(bitmap + id / 8) & (1 << id % 8));
    free_block(fs, bitmap);
    return ans;
}

__u8 is_inode_used(struct ext2_file_system *fs, __u32 id) {
    __u32 group = (id - 1) / fs->sb.s_inodes_per_group;
    id = (id - 1) % fs->sb.s_inodes_per_group;
    void *bitmap = block_alloc(fs, fs->bgdt[group].bg_inode_bitmap);
    __u8 ans = !!(*(__u8*)(bitmap + id / 8) & (1 << id % 8));
    free_block(fs, bitmap);
    return ans;
}

int put_inode(struct ext2_file_system *me, struct inode inode_, __u32 inode_dir_id) {
    return 0;
}

int delete_inode(struct ext2_file_system *me, __u32 inode) {
    return 0;
}