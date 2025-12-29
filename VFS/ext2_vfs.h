#pragma once

#include "ext2_fs.h"
#include "inode_gate.h"
#include "dir_gate.h"
#include "inode.h"
#include "inode_table.h"
#include "fd_table.h"

#include <time.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define ROOT_DIR_INODE_ID 2

#define DIR_TYPE_DIR 2

struct ext2_vfs {
    struct ext2_file_system *fs;
    struct inode_table inode_table;
    struct fd_table fd_table;

    __u16 user_id;
    __u16 group_id;
};

void ext2_vfs_init(struct ext2_vfs *vfs, const char *file);
void ext2_vfs_destroy(struct ext2_vfs *vfs);

__u32 ext2_vfs_mount(struct ext2_vfs *vfs, const char *path, __u16 user_id, __u16 group_id);
__u32 ext2_vfs_umount(struct ext2_vfs *vfs);

__s32 ext2_vfs_list(struct ext2_vfs *vfs, const char *path, void *buf);
__s32 ext2_vfs_mkdir(struct ext2_vfs *vfs, const char *path, const char *name);
// directories must by empty to be deleted
__s32 ext2_vfs_unlink(struct ext2_vfs *vfs, const char *path);

__u32 ext2_vfs_ln(struct ext2_vfs *vfs, __u32 option, const char *original_path, const char *path_to_target);

__s32 ext2_vfs_touch(struct ext2_vfs *vfs, __u32 option, const char *path);
// __s32 ext2_vfs_chmod();

__s32 ext2_vfs_open(struct ext2_vfs *vfs, const char *path, int flags);
__s32 ext2_vfs_read(struct ext2_vfs *vfs, __u32 fd, void *buf, __u32 count);
__s32 ext2_vfs_write(struct ext2_vfs *vfs, __u32 fd, const void *buf, __u32 count);
__s32 ext2_vfs_close(struct ext2_vfs *vfs, __u32 fd);

__s32 ext2_vfs_ftruncate(struct ext2_vfs *vfs, __u32 fd, __u32 length);