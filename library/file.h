#pragma once

#include "inode.h"
#include "ext2_fs.h"

struct file {
    struct ext2_file_system *fs;
    struct inode inode;
    void** table;
    __u32 count;
};

int open_file(struct file *f, struct ext2_file_system *me, __u32 id) {
    f->fs = me;
    f->inode = read_inode(f->fs, id);
    f->table = NULL;
    f->count = 0;
    return 0;
}

void* mmap_file();

void munmap_file();

void close_file(struct file *f) {
    f->fs = NULL;
}