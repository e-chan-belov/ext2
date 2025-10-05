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

    debug_bgdt(current.bgdt[0]);

    struct inode first = read_inode(&current, 2);
    debug_inode(first);

    ext2_file_system_destroy(&current);
    return 0;
}