#include "dir.h"

__u32 virtual_dir_init(struct virtual_dir *vd, struct ext2_file_system *fs, struct inode *inode){
    virtual_inode_init(&(vd->vi), fs, inode);
    return 0;
}
__u32 virtual_dir_destroy(struct virtual_dir *vd) {
    virtual_inode_destroy(&(vd->vi));
    return 0;
}

void virtual_dir_next_entry() {
    
}