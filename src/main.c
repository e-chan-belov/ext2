#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/mman.h>

#include "super_block.h"
#include "bgdt.h"
#include "inode.h"
#include "types.h"
#include "dir.h"
#include "debug.h"
#include "ext2_fs.h"




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
        debug_bgdt(current.bgdt[i]);
    }
    

    struct inode first = read_inode(&current, 2);
    debug_inode(first);

    struct file root;
    file_init(&root, &current, 2);
    read_open_file(&root);
    struct ext2_dir_entry *dir = (struct ext2_dir_entry*)(root.ptr);
    debug_ext2_dir_entry(dir);
    close_file(&root);

    ext2_file_system_destroy(&current);
    return 0;
}