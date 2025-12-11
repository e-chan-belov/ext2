#pragma once

#include "ext2_fs.h"
#include "inode_table.h"
#include "fd_table.h"

struct ext2_vfs {
    struct ext2_file_system *fs;
    struct inode_table inode_table;
    struct fd_table fd_table;
};

void ext2_vfs_init(struct ext2_vfs *vfs, const char *file);
void ext2_vfs_destroy(struct ext2_vfs *vfs);

__u32 ext2_vfs_mount(struct ext2_vfs *vfs, const char *path);
__u32 ext2_vfs_umount(struct ext2_vfs *vfs);

__s32 ext2_vfs_list(struct ext2_vfs *vfs, const char *path, void *buf);
__s32 ext2_vfs_mkdir(struct ext2_vfs *vfs, const char *path);
// directories must by empty to be deleted
__s32 ext2_vfs_unlink(struct ext2_vfs *vfs, const char *path);

__s32 ext2_touch(struct ext2_vfs *vfs, const char *path);

__s32 ext2_vfs_open(struct ext2_vfs *vfs, const char *path, int flags);
__s32 ext2_vfs_read(struct ext2_vfs *vfs, __u32 fd, void *buf, __u32 count);
__s32 ext2_vfs_write(struct ext2_vfs *vfs, __u32 fd, const void *buf, __u32 count);
__s32 ext2_vfs_close(struct ext2_vfs *vfs, __u32 fd);

__s32 ext2_vfs_ftruncate(struct ext2_vfs *vfs, __u32 fd, __u32 length);