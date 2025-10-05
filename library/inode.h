#pragma once
#include "types.h"
#include "ext2_fs.h"
#include "super_block.h"

struct inode {
    __u16 i_mode; /* File mode */
    __u16 i_uid; /* Owner Uid */
    __u32 i_size; /* Size in bytes */
    __u32 i_atime; /* Access time */
    __u32 i_ctime; /* Creation time */
    __u32 i_mtime; /* Modification time */
    __u32 i_dtime; /* Deletion Time */
    __u16 i_gid; /* Group Id */
    __u16 i_links_count; /* Links count */
    __u32 i_blocks; /* Blocks count */
    __u32 i_flags; /* File flags */
    __u32 i_osd1; /* OS dependent 1 */
    __u32 i_block[15]; /* Pointers to blocks */
    __u32 i_generation; /* File version (for NFS) */
    __u32 i_file_acl; /* File ACL */
    __u32 i_dir_acl; /* Directory ACL / high 32 bits of file size (ext2/3 vs ext4) */
    __u32 i_faddr; /* Fragment address */
    __u8  i_osd2[12]; /* OS dependent 2 */
};

