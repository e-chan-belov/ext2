#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <fcntl.h>

#include "types.h"
#include "super_block.h"
#include "bgdt.h"
#include "inode.h"

struct ext2_file_system {
    __u32 fd;

    struct super_block sb;
    struct block_group_descriptor* bgdt;

    __u32 block_size;
    __u32 groups_count;

    /* cache section */
    __u32 block_bitmap_id;
    void *last_block_bitmap;
    __u32 inode_bitmap_id;
    void *last_inode_bitmap;
};

__u32 ext2_file_system_create(const char *file);
__u32 ext2_file_system_init(struct ext2_file_system *fs, const char *file);
__u32 ext2_file_system_destroy(struct ext2_file_system *fs);

struct super_block get_super_block(struct ext2_file_system *fs);
__u32 set_super_block(struct ext2_file_system *fs, struct super_block sb);

__u32 get_block_size_from_fs(struct ext2_file_system *fs);

struct block_group_descriptor get_bgd(struct ext2_file_system *fs, __u32 index);
__u32 set_bgd(struct ext2_file_system *fs, struct block_group_descriptor bgd,__u32 index);

void* block_mmap(struct ext2_file_system *fs, __u32 id);
__u32 block_munmap(struct ext2_file_system *fs, void *ptr);

__u8 is_block_used(struct ext2_file_system *fs, __u32 id);
__u8 is_inode_used(struct ext2_file_system *fs, __u32 id);

__u8 set_bit_block_bitmap(struct ext2_file_system *fs, __u32 id);
__u8 set_bit_inode_bitmap(struct ext2_file_system *fs, __u32 id);
__u8 unset_bit_block_bitmap(struct ext2_file_system *fs, __u32 id);
__u8 unset_bit_inode_bitmap(struct ext2_file_system *fs, __u32 id);

__u32 block_alloc(struct ext2_file_system *fs, __u32 block);
__u32 free_block(struct ext2_file_system *fs, __u32 block);

__u32 first_free_block(struct ext2_file_system *fs, __u32 hint);
__u32 first_free_inode(struct ext2_file_system *fs, __u32 hint);

struct inode read_inode(struct ext2_file_system *fs, __u32 inode);
__u32 inode_alloc(struct ext2_file_system *fs, __u32 inode);
__u32 free_inode(struct ext2_file_system *fs, __u32 inode);
__u32 put_inode(struct ext2_file_system *fs, struct inode inode_, __u32 id);