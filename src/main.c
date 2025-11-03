#include <stdio.h>
#include <stdlib.h>


#include "bgdt.h"
#include "inode.h"
#include "types.h"
#include "fs_debug.h"
#include "ext2_fs.h"
#include "dir_gate.h"




int main() {
    struct ext2_file_system current;
    int err = ext2_file_system_init(&current, "/workspaces/ext2_test_first/disk.img");
    if (err < 0) { 
        printf("%d\n", err);
        return err; 
    }

    debug_super_block(current.sb);
    printf("current.groups_count: %u\n", current.groups_count);
    int i;
    for (i = 0; i < current.groups_count; i++) {
        printf("Block Group Descriptor number: %u\n", i);
        debug_bgd(current.bgdt[i]);
    }
    

    /*struct inode first = read_inode(&current, 12);
    debug_inode(first);
    debug_inode_gate(12, &current);*/

    /*__u32 inode_ = first_free_inode(&current, 12);
    inode_alloc(&current, inode_);
    struct inode test_inode;
    test_inode.i_links_count = 0;
    put_inode(&current, test_inode, inode_);
    add_new_entry(&current, 2, "TEST.c", 1, inode_);
    debug_inode(read_inode(&current, inode_));*/

    struct dir_gate dg;
    dir_gate_init(&dg, &current, 2);

    next_entry(&dg);
    next_entry(&dg);

    //printf("%d\n", is_inode_used(&current, 12));
    
    struct ext2_dir_entry cur_dentry = get_current_entry(&dg);
    debug_ext2_dir_entry(&cur_dentry);

    printf("TESTING!\n");


    ext2_dir_entry_destroy(&cur_dentry);

    next_entry(&dg);
    cur_dentry = get_current_entry(&dg);
    debug_ext2_dir_entry(&cur_dentry);
    ext2_dir_entry_destroy(&cur_dentry);

    //delete_current_entry(&dg);
    //free_inode(&current, 12);
    printf("%d\n", is_inode_used(&current, 12));
    //debug_inode(read_inode(&current, 12));
    dir_gate_destroy(&dg);
    
    ext2_file_system_destroy(&current);
    return 0;
}