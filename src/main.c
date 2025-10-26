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
    int err = ext2_file_system_init(&current, "/mnt/disk.img");
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
    

    struct inode first = read_inode(&current, 13);
    debug_inode(first);
/*
    struct file root;
    file_init(&root, &current, 2);
    read_open_file(&root);
    struct dir dir;
    if (dir_init(&dir, &root) < 0) { return -1; }
    next_entry(&dir);
    next_entry(&dir);
    next_entry(&dir);
    next_entry(&dir);
    next_entry(&dir);
    debug_ext2_dir_entry(dir.current_entry);
    close_file(&root);*/

    debug_inode_gate(13, &current);

    struct dir_gate dg;
    dir_gate_init(&dg, &current, 2);

    next_entry(&dg);
    next_entry(&dg);
    next_entry(&dg);
    next_entry(&dg);
    next_entry(&dg);
    next_entry(&dg);

    debug_ext2_dir_entry(get_current_entry(&dg));

    dir_gate_destroy(&dg);
    ext2_file_system_destroy(&current);
    return 0;
}