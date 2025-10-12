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
        debug_bgd(current.bgdt[i]);
    }
    

    struct inode first = read_inode(&current, 25603);
    debug_inode(first);

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
    close_file(&root);

    struct super_block temp;
    temp = *(struct super_block*)debug_via_mmap(current.fd, 1024, current.sb.s_blocks_per_group * current.block_size);
    debug_super_block(temp);
    ext2_file_system_destroy(&current);
    return 0;
}