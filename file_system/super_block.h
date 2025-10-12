#pragma once
#include "types.h"

struct super_block {
    __u32 s_inodes_count; /* Total amout of Inodes in FS */
    __u32 s_blocks_count; /* Total amout of Blocks in FS */
    __u32 s_r_blocks_count; /* Reserved blocks count in FS */
    __u32 s_free_blocks_count; /* Free blocks count in FS */
    __u32 s_free_inodes_count; /* Free inodes count in FS */
    __u32 s_first_data_block; /* First Data Block index */
    __u32 s_log_block_size; /* Block size */
    __s32 s_log_frag_size; /* Fragment size */
    __u32 s_blocks_per_group; /* # Blocks per group */
    __u32 s_frags_per_group; /* # Fragments per group */
    __u32 s_inodes_per_group; /* # Inodes per group */
    __u32 s_mtime; /* [last] Mount time */
    __u32 s_wtime; /* Write time */
    __u16 s_mnt_count; /* Mount count */
    __s16 s_max_mnt_count; /* Maximal mount count [before check]*/
    __u16 s_magic; /* Magic signature */
    __u16 s_state; /* File system state */
    __u16 s_errors; /* Behaviour when detecting errors */
    __u16 s_minor_rev_level; /* minor revision level */
    __u32 s_lastcheck; /* time of last check */
    __u32 s_checkinterval; /* max. time between checks */
    __u32 s_creator_os; /* OS */
    __u32 s_rev_level; /* Revision level */
    __u16 s_def_resuid; /* Default uid for reserved blocks*/
    __u16 s_def_resgid; /* Default gid for reserved blocks */
    __u32 s_first_ino; /* index to the first inode useable for standard files */
    __u16 s_inode_size; /* size of the inode structure */
    __u16 s_block_group_nr; /* block group number hosting this superblock structure */
    __u32 s_feature_compat; /* bitmask of compatible features */
    __u32 s_feature_incompat; /* bitmask of incompatible features */
    __u32 s_feature_ro_compat; /* bitmask of “read-only” features */
    __u8 s_uuid[16]; /* value used as the volume id */
    __u16 s_volume_name; /* volume name, mostly unusued */
    __u8 s_last_mounted[8]; /* directory path where the file system was last mounted */
    __u32 s_algo_bitmap; /* value used by compression algorithms to determine the compression method(s) used */
    __u8 s_prealloc_blocks; /* representing the number of blocks the implementation should attempt to pre-allocate when creating a new regular file */
    __u8 s_prealloc_dir_blocks; /* value representing the number of blocks the implementation should attempt to pre-allocate when creating a new directory */
    __u16 s_journal_uuid; /* containing the uuid of the journal superblock */
    __u32 s_journal_inum; /*  inode number of the journal file */
    __u32 s_journal_dev; /* device number of the journal file */
    __u32 s_last_orphan; /* inode number, pointing to the first inode in the list of inodes to delete */
    __u32 s_hash_seed[4]; /* An array of 4 32bit values containing the seeds used for the hash algorithm for directory indexing */
    __u8 s_def_hash_version; /* default hash version used for directory indexing */
    __u32 s_default_mount_options; /* default mount options for this file system */
    __u32 s_first_meta_bg; /* MAYBE UNUSED!!! indicating the block group ID of the first meta block group */
};

__u32 get_block_size(struct super_block *sb);

__u32 get_frag_size(struct super_block *sb);