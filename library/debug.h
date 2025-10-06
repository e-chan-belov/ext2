#pragma once


#include "super_block.h"
#include "bgdt.h"
#include "inode.h"
#include "dir.h"

void debug_super_block(struct super_block sb) {
    printf("s_inodes_count: %u\n", sb.s_inodes_count);
    printf("s_blocks_count: %u\n", sb.s_blocks_count);
    printf("s_r_blocks_count: %u\n", sb.s_r_blocks_count);
    printf("s_free_blocks_count: %u\n", sb.s_free_blocks_count);
    printf("s_free_inodes_count: %u\n", sb.s_free_inodes_count);
    printf("s_first_data_block: %u\n", sb.s_first_data_block);
    printf("s_log_block_size: %u\n", sb.s_log_block_size);
    printf("s_log_frag_size: %d\n", sb.s_log_frag_size);
    printf("s_blocks_per_group: %u\n", sb.s_blocks_per_group);
    printf("s_frags_per_group: %u\n", sb.s_frags_per_group);
    printf("s_inodes_per_group: %u\n", sb.s_inodes_per_group);
    printf("s_mtime: %u\n", sb.s_mtime);
    printf("s_wtime: %u\n", sb.s_wtime);
    printf("s_mnt_count: %u\n", sb.s_mnt_count);
    printf("s_max_mnt_count: %d\n", sb.s_max_mnt_count);
    printf("s_magic: %u\n", sb.s_magic);
    printf("s_state: %u\n", sb.s_state);
    printf("s_errors: %u\n", sb.s_errors);
    printf("s_minor_rev_level: %u\n", sb.s_minor_rev_level);
    printf("s_lastcheck: %u\n", sb.s_lastcheck);
    printf("s_checkinterval: %u\n", sb.s_checkinterval);
    printf("s_creator_os: %u\n", sb.s_creator_os);
    printf("s_rev_level: %u\n", sb.s_rev_level);
    printf("s_def_resuid: %u\n", sb.s_def_resuid);
    printf("s_def_resgid: %u\n", sb.s_def_resgid);
    printf("s_first_ino: %u\n", sb.s_first_ino);
    printf("s_inode_size: %u\n", sb.s_inode_size);
    printf("s_block_group_nr: %u\n", sb.s_block_group_nr);
    printf("s_feature_compat: %u\n", sb.s_feature_compat);
    printf("s_feature_incompat: %u\n", sb.s_feature_incompat);
    printf("s_feature_ro_compat: %u\n", sb.s_feature_ro_compat);
    
    printf("s_uuid: ");
    int i;
    for(i = 0; i < 16; i++) {
        printf("%02x", sb.s_uuid[i]);
    }
    printf("\n");
    
    printf("s_volume_name: %u\n", sb.s_volume_name);
    
    printf("s_last_mounted: ");
    for(i = 0; i < 8; i++) {
        printf("%02x", sb.s_last_mounted[i]);
    }
    printf("\n");
    
    printf("s_algo_bitmap: %u\n", sb.s_algo_bitmap);
    printf("s_prealloc_blocks: %u\n", sb.s_prealloc_blocks);
    printf("s_prealloc_dir_blocks: %u\n", sb.s_prealloc_dir_blocks);
    printf("s_journal_uuid: %u\n", sb.s_journal_uuid);
    printf("s_journal_inum: %u\n", sb.s_journal_inum);
    printf("s_journal_dev: %u\n", sb.s_journal_dev);
    printf("s_last_orphan: %u\n", sb.s_last_orphan);
    
    printf("s_hash_seed[0]: %u\n", sb.s_hash_seed[0]);
    printf("s_hash_seed[1]: %u\n", sb.s_hash_seed[1]);
    printf("s_hash_seed[2]: %u\n", sb.s_hash_seed[2]);
    printf("s_hash_seed[3]: %u\n", sb.s_hash_seed[3]);
    
    printf("s_def_hash_version: %u\n", sb.s_def_hash_version);
    printf("s_default_mount_options: %u\n", sb.s_default_mount_options);
    printf("s_first_meta_bg: %u\n", sb.s_first_meta_bg);
}

void debug_bgdt(struct block_group_descriptor_table current_bgdt) {
    printf("bg_block_bitmap: %u\n", current_bgdt.bg_block_bitmap);
    printf("bg_inode_bitmap: %u\n", current_bgdt.bg_inode_bitmap);
    printf("bg_inode_table: %u\n", current_bgdt.bg_inode_table);
    printf("bg_free_blocks_count: %u\n", current_bgdt.bg_free_blocks_count);
    printf("bg_free_inodes_count: %u\n", current_bgdt.bg_free_inodes_count);
    printf("bg_used_dirs_count: %u\n", current_bgdt.bg_used_dirs_count);
    printf("bg_pad: %u\n", current_bgdt.bg_pad);
    printf("bg_reserved[0]: %u\n",  current_bgdt.bg_reserved[0]);
    printf("bg_reserved[1]: %u\n",  current_bgdt.bg_reserved[1]);
    printf("bg_reserved[2]: %u\n",  current_bgdt.bg_reserved[2]);
}

void debug_inode(struct inode node) {
    printf("i_mode: %u\n", node.i_mode);
    printf("i_uid: %u\n", node.i_uid);
    printf("i_size: %u\n", node.i_size);
    printf("i_atime: %u\n", node.i_atime);
    printf("i_ctime: %u\n", node.i_ctime);
    printf("i_mtime: %u\n", node.i_mtime);
    printf("i_dtime: %u\n", node.i_dtime);
    printf("i_gid: %u\n", node.i_gid);
    printf("i_links_count: %u\n", node.i_links_count);
    printf("i_blocks: %u\n", node.i_blocks);
    printf("i_flags: %u\n", node.i_flags);
    printf("i_osd1: %u\n", node.i_osd1);
    
    printf("i_block[0]: %u\n", node.i_block[0]);
    printf("i_block[1]: %u\n", node.i_block[1]);
    printf("i_block[2]: %u\n", node.i_block[2]);
    printf("i_block[3]: %u\n", node.i_block[3]);
    printf("i_block[4]: %u\n", node.i_block[4]);
    printf("i_block[5]: %u\n", node.i_block[5]);
    printf("i_block[6]: %u\n", node.i_block[6]);
    printf("i_block[7]: %u\n", node.i_block[7]);
    printf("i_block[8]: %u\n", node.i_block[8]);
    printf("i_block[9]: %u\n", node.i_block[9]);
    printf("i_block[10]: %u\n", node.i_block[10]);
    printf("i_block[11]: %u\n", node.i_block[11]);
    printf("i_block[12]: %u\n", node.i_block[12]);
    printf("i_block[13]: %u\n", node.i_block[13]);
    printf("i_block[14]: %u\n", node.i_block[14]);
    
    printf("i_generation: %u\n", node.i_generation);
    printf("i_file_acl: %u\n", node.i_file_acl);
    printf("i_dir_acl: %u\n", node.i_dir_acl);
    printf("i_faddr: %u\n", node.i_faddr);
    
    printf("i_osd2: ");
    int i;
    for(i = 0; i < 12; i++) {
        printf("%02x", node.i_osd2[i]);
    }
    printf("\n");
}

void debug_ext2_dir_entry(struct ext2_dir_entry *entry) {
    printf("inode: %u\n", entry->inode);
    printf("rec_len: %u\n", entry->rec_len);
    printf("name_len: %u\n", entry->name_len);
    printf("file_type: %u\n", entry->file_type);
    
    printf("name: ");
    int i;
    for (i = 0; i < entry->name_len; i++) {
        printf("%c", entry->name[i]);
    }
    printf("\n");
    
    if (entry->name_len > 0 && entry->name[entry->name_len - 1] == '\0') {
        printf("name (as string): %s\n", entry->name);
    }
}