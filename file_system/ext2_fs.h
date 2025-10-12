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
    int fd;
    struct super_block sb;
    __u32 block_size;
    struct block_group_descriptor* bgdt;
    __u32 groups_count;
};

int ext2_file_system_create(const char *file);
int ext2_file_system_init(struct ext2_file_system *me, const char *file);
void ext2_file_system_destroy(struct ext2_file_system *me);

void ext2_file_system_mount(struct ext2_file_system *fs, const char *file);
void ext2_file_system_unmount(struct ext2_file_system *fs);

void* block_mmap(struct ext2_file_system *fs, __u32 id);
void block_munmap(struct ext2_file_system *fs, void *ptr);

struct inode read_inode(struct ext2_file_system *me, __u32 inode);

__u8 is_block_used(struct ext2_file_system *fs, __u32 id);
__u8 is_inode_used(struct ext2_file_system *fs, __u32 id);

int put_inode(struct ext2_file_system *me, struct inode inode_, __u32 inode_dir_id);
int delete_inode(struct ext2_file_system *me, __u32 inode);