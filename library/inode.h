#pragma once
#include "types.h"
#include "super_block.h"

/*#define S_IFSOCK 0xC000  // socket
#define S_IFLNK  0xA000  // symbolic link
#define S_IFREG  0x8000  // regular file
#define S_IFBLK  0x6000  // block device
#define S_IFDIR  0x4000  // directory
#define S_IFCHR  0x2000  // character device
#define S_IFIFO  0x1000  // FIFO (pipe)

#define S_FF 0xF000 // file format

#define IS_SOCK(i_mode) (((i_mode & S_FF) & S_IFSOCK) == S_IFSOCK)
#define IS_LNK(i_mode) (((i_mode & S_FF) & S_IFLNK) == S_IFLNK)
#define IS_REG(i_mode) (((i_mode & S_FF) & S_IFREG) == S_IFREG)
#define IS_BLK(i_mode) (((i_mode & S_FF) & S_IFBLK) == S_IFBLK)
#define IS_DIR(i_mode) (((i_mode & S_FF) & S_IFDIR) == S_IFDIR)
#define IS_CHR(i_mode) (((i_mode & S_FF) & S_IFCHR) == S_IFCHR)
#define IS_FIFO(i_mode) (((i_mode & S_FF) & S_IFIFO) == S_IFIFO)

#define S_ISUID 0x0800  // Set process User ID
#define S_ISGID 0x0400  // Set process Group ID
#define S_ISVTX 0x0200  // sticky bit

#define S_IRUSR 0x0100  // user read
#define S_IWUSR 0x0080  // user write
#define S_IXUSR 0x0040  // user execute
#define S_IRGRP 0x0020  // group read
#define S_IWGRP 0x0010  // group write
#define S_IXGRP 0x0008  // group execute
#define S_IROTH 0x0004  // others read
#define S_IWOTH 0x0002  // others write
#define S_IXOTH 0x0001  // others execute
*/

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

int is_dir(struct inode *inode) {
    return (((inode->i_mode & 0xF000) & 0x4000) == 0x4000);
}