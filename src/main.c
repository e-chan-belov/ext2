#include <stdio.h>
#include <stdlib.h>


#include "bgdt.h"
#include "inode.h"
#include "types.h"
#include "fs_debug.h"
#include "ext2_fs.h"
#include "dir_gate.h"
#include "rb_tree.h"
#include "vfs_debug.h"
#include "ext2_vfs.h"

struct inode global;

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
    

    struct inode first = read_inode(&current, 12);
    debug_inode(first);
    debug_inode_gate(12, &current);

    //struct inode test_file = create_default_file(0, 0, 0x8000);
    /*__u32 inode_ = first_free_inode(&current, 12);
    inode_alloc(&current, inode_);
    put_inode(&current, test_file, inode_);
    add_new_entry(&current, 2, "TEST.c", 1, inode_);
    debug_inode(read_inode(&current, inode_));*/

    struct dir_gate dg;
    dir_gate_init(&dg, &current, 2);

    next_entry(&dg);
    next_entry(&dg);
    next_entry(&dg);

    //entry_current_dir(&dg);

    //printf("%d\n", is_inode_used(&current, 12));
    
    struct ext2_dir_entry cur_dentry = get_current_entry(&dg);
    debug_ext2_dir_entry(&cur_dentry);

    printf("TESTING!\n");


    
    next_entry(&dg);
    next_entry(&dg);
    next_entry(&dg);

    cur_dentry = get_current_entry(&dg);
    debug_ext2_dir_entry(&cur_dentry);
    
    //delete_current_entry(&dg);

    printf("%d\n", is_inode_used(&current, 13));
    dir_gate_destroy(&dg);

    //struct inode test_dir = read_inode(&current, 13);
    //debug_inode(test_dir);
    //printf("%u\n", test_dir.i_size);

    struct ext2_vfs vfs;
    vfs.fs = &current;
    vfs.user_id = 0;
    vfs.group_id = 0;
    ext2_vfs_mkdir(&vfs, "/", "test2");

    /*struct inode file1 = create_default_file(vfs.user_id, vfs.group_id, EXT2_S_IFREG);
    struct inode file2 = create_default_file(vfs.user_id, vfs.group_id, EXT2_S_IFREG);
    struct inode file3 = create_default_file(vfs.user_id, vfs.group_id, EXT2_S_IFREG);

    __u32 inode_id1 = first_free_inode(&current, 12);
    inode_alloc(vfs.fs, inode_id1);
    put_inode(vfs.fs, file1, inode_id1);

    __u32 inode_id2 = first_free_inode(&current, 12);
    inode_alloc(vfs.fs, inode_id2);
    put_inode(vfs.fs, file2, inode_id2);

    __u32 inode_id3 = first_free_inode(&current, 12);
    inode_alloc(vfs.fs, inode_id3);
    put_inode(vfs.fs, file3, inode_id3);

    add_new_entry(vfs.fs, 2, "file1.cpp", 1, inode_id1);
    add_new_entry(vfs.fs, 2, "file2.cpp", 1, inode_id2);
    add_new_entry(vfs.fs, 2, "file3.cpp", 1, inode_id3);*/


    //create_default_dir(&vfs, 2, "test");
    //printf("%u\n", find_inode_id_by_name_in_dir(&vfs, ".", 13));
    
    ext2_file_system_destroy(&current);
    return 0;
}