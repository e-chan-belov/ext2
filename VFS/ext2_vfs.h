#pragma once

#include "ext2_fs.h"

struct ext2_vfs {
    struct ext2_file_system *fs;
};

void ext2_vfs_init(struct ext2_vfs *vfs, const char *file);
void ext2_vfs_destroy(struct ext2_vfs *vfs);

void ext2_vfs_mount(struct ext2_vfs *vfs, const char *path);
void ext2_vfs_unmount(struct ext2_vfs *vfs);

__s32 ext2_vfs_list(struct ext2_vfs *vfs, const char *path, void *buf);
__s32 ext2_vfs_mkdir(struct ext2_vfs *vfs, const char *path, const char *name);
// directories must by empty to be deleted
__s32 ext2_vfs_unlink(struct ext2_vfs *vfs, const char *path);

__s32 ext2_vfs_open(struct ext2_vfs *vfs, const char *path, int flags);
__s32 ext2_vfs_read(struct ext2_vfs *vfs, __u32 fd, void *buf, __u32 count);
__s32 ext2_vfs_write(struct ext2_vfs *vfs, int fd, const void *buf, __u32 count);
__s32 ext2_vfs_close(struct ext2_vfs *vfs, __u32 fd);

__s32 ext2_vfs_ftruncate(struct ext2_vfs *vfs, __u32 fd, __u32 length);