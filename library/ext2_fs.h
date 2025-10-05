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

struct virtual_disk_info {
    int fd;
    int ln;
};

struct ext2_file_system {
    struct virtual_disk_info vd_info;
    void* pointer_to_first_block;
    struct super_block first_super_block;
    __u32 block_size;
    struct block_group_descriptor_table* bgdt;
    __u32 groups_count;
};

int ext2_file_system_init(struct ext2_file_system *me, const char *file) {
    int fd = open(file, O_RDONLY);
    if (fd < 0) { return -1; }
    int ln = lseek(fd, 0, SEEK_END);
    if (ln < 0) { return -2; }
    void *ptr = mmap(NULL, ln, PROT_READ, MAP_PRIVATE, fd, 0);
    if (ptr == MAP_FAILED) { return -3; }

    me->vd_info.fd = fd;
    me->vd_info.ln = ln;
    me->pointer_to_first_block = ptr;
    me->first_super_block = *(struct super_block*)(me->pointer_to_first_block + 1024);
    me->block_size = get_block_size(&me->first_super_block);
    me->bgdt = (struct block_group_descriptor_table*)(me->pointer_to_first_block + me->block_size);
    me->groups_count = CEIL_DIV(me->first_super_block.s_blocks_count, me->first_super_block.s_blocks_per_group);
    return 0;
}

void ext2_file_system_destroy(struct ext2_file_system *me) {
    munmap(me->pointer_to_first_block, me->vd_info.ln);
    close(me->vd_info.fd);
}

// returns a pointer to the requested inode structure PLEASE DEBUG
struct inode* locate_a_local_inode(struct ext2_file_system *me, __u32 inode) {
    __u32 block_group_index = (inode - 1) / me->first_super_block.s_inodes_per_group;
    void* ptr_first_byte_requested_group = me->pointer_to_first_block + me->block_size * me->first_super_block.s_blocks_per_group * block_group_index;
    __u32 local_inode_index = (inode - 1) % me->first_super_block.s_inodes_per_group;
    void* ptr_inode_table = (struct inode*)(ptr_first_byte_requested_group + me->bgdt[block_group_index].bg_inode_table * me->block_size);
    return ptr_inode_table + local_inode_index * me->first_super_block.s_inode_size;
}

void* locate_block_group_by_index(struct ext2_file_system *me,__u32 block_group_index) {
    return me->pointer_to_first_block + block_group_index * me->first_super_block.s_blocks_per_group * get_block_size(me->pointer_to_first_block);
}