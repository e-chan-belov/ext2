#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <fcntl.h>

#include "types.h"
#include "super_block.h"
#include "bgdt.h"
#include "inode.h"
#include "virtual_inode.h"
#include "dir.h"

struct ext2_file_system {
    __u32 fd;
    struct super_block sb;
    __u32 block_size;
    struct block_group_descriptor* bgdt;
    __u32 groups_count;
};

__u32 ext2_file_system_create(const char *file);
__u32 ext2_file_system_init(struct ext2_file_system *me, const char *file);
void ext2_file_system_destroy(struct ext2_file_system *me);

void ext2_file_system_mount(struct ext2_file_system *fs, const char *file);
void ext2_file_system_unmount(struct ext2_file_system *fs);

struct super_block get_super_block(struct ext2_file_system *fs);
void set_super_block(struct ext2_file_system *fs, struct super_block sb);

struct block_group_descriptor get_bgd(struct ext2_file_system *fs, __u32 index);
void set_bgd(struct ext2_file_system *fs, struct block_group_descriptor bgd,__u32 index);

void* block_mmap(struct ext2_file_system *fs, __u32 id);
void block_munmap(struct ext2_file_system *fs, void *ptr);

__u8 is_block_used(struct ext2_file_system *fs, __u32 id);
__u8 is_inode_used(struct ext2_file_system *fs, __u32 id);

__u32 block_alloc(struct ext2_file_system *fs, __u32 block);
__u32 free_block(struct ext2_file_system *fs, __u32 block);

struct inode read_inode(struct ext2_file_system *me, __u32 inode);
__u32 inode_alloc(struct ext2_file_system *fs, __u32 inode);
__u32 put_inode(struct ext2_file_system *me, struct inode inode_, __u32 inode_dir_id);
__u32 free_inode(struct ext2_file_system *me, __u32 inode);

struct virtual_node open_inode(struct ext2_file_system *fs, __u32 inode);

struct dir open_dir(struct ext2_file_system *fs, __u32 inode);