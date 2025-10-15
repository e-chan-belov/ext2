#include "ext2_fs.h"

__u32 ext2_file_system_create(const char *file) {
    return 0;
}

__u32 ext2_file_system_init(struct ext2_file_system *me, const char *file) {
    __u32 fd = open(file, O_RDWR);

    __u32 err;
    void *ptr = mmap(NULL, 2048, PROT_READ, MAP_PRIVATE, fd, 0);
    if (ptr == MAP_FAILED) {
        return -2;
    }

    me->fd = fd;
    me->sb = *(struct super_block*)(ptr + 1024);

    err = munmap(ptr, 1024);
    if (err < 0) {
        return -3;
    }

    me->block_size = get_block_size(&me->sb);
    me->groups_count = CEIL_DIV(me->sb.s_blocks_count, me->sb.s_blocks_per_group);

    __u32 sz = me->groups_count * sizeof(struct block_group_descriptor);
    me->bgdt = malloc(sz);
    ptr = mmap(NULL, sz, PROT_READ, MAP_PRIVATE, fd, me->block_size); // THIS MAY CAUSE MAP_FAILED!!!!!
    if (ptr == MAP_FAILED) {
        return -4;
    }

    struct block_group_descriptor *tmp_ptr = ptr;
    __u32 i;
    for (i = 0; i < me->groups_count; i++, tmp_ptr++) {
        me->bgdt[i] = *tmp_ptr;
    }

    err = munmap(ptr, sz);
    if (err < 0) {
        return -5;
    }

    me->block_bitmap_id = 0;
    me->last_block_bitmap = NULL;
    me->inode_bitmap_id = 0;
    me->last_inode_bitmap = NULL;

    return 0;
}

void ext2_file_system_destroy(struct ext2_file_system *me) {
    free(me->bgdt);
    close(me->fd);
}

// please be aware that it can fail because of a page size
// it doesn't fail for 4K block size
void* block_mmap(struct ext2_file_system *fs, __u32 id) {
    void *ptr = mmap(NULL, fs->block_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fs->fd, fs->block_size * id);
    if (ptr == MAP_FAILED) {
        printf("BLOCK_MMAP FAILURE!!! BLOCK ID: %u AND BLOCK SIZE: %u", id, fs->block_size);
        return NULL;
    }
    return ptr;
}

void block_munmap(struct ext2_file_system *fs, void *ptr) {
    munmap(ptr, fs->block_size);
}

// returns a copy of the requested inode structure
struct inode read_inode(struct ext2_file_system *me, __u32 inode) {
    __u32 block_group_index = (inode - 1) / me->sb.s_inodes_per_group;
    __u32 local_inode_index = (inode - 1) % me->sb.s_inodes_per_group;
    __u32 id = me->bgdt[block_group_index].bg_inode_table + local_inode_index / (me->block_size / me->sb.s_inode_size);
    void *ptr = block_mmap(me, id);
    struct inode temp = *(struct inode*)(ptr + me->sb.s_inode_size * (local_inode_index % (me->block_size / me->sb.s_inode_size)));
    block_munmap(me, ptr);
    return temp;
}

__u8 is_block_used(struct ext2_file_system *fs, __u32 id) {
    __u32 group = id / fs->sb.s_blocks_per_group;
    id = id % fs->sb.s_blocks_per_group;
    
    if (fs->block_bitmap_id == 0 || fs->last_block_bitmap == NULL) {
        fs->block_bitmap_id = fs->bgdt[group].bg_block_bitmap;
        fs->last_block_bitmap = block_mmap(fs, fs->bgdt[group].bg_block_bitmap);
    }
    else if (fs->block_bitmap_id != fs->bgdt[group].bg_block_bitmap) {
        block_munmap(fs, fs->last_block_bitmap);
        fs->block_bitmap_id = fs->bgdt[group].bg_block_bitmap;
        fs->last_block_bitmap = block_mmap(fs, fs->bgdt[group].bg_block_bitmap);
    }
     
    __u8 ans = !!(*(__u8*)(fs->last_block_bitmap + id / 8) & (1 << id % 8));
    return ans;
}

__u8 is_inode_used(struct ext2_file_system *fs, __u32 id) { // todo
    __u32 group = (id - 1) / fs->sb.s_inodes_per_group;
    id = (id - 1) % fs->sb.s_inodes_per_group;
   
    if (fs->inode_bitmap_id == 0 || fs->last_inode_bitmap == NULL) {
        fs->inode_bitmap_id = fs->bgdt[group].bg_inode_bitmap;
        fs->last_inode_bitmap = block_mmap(fs, fs->bgdt[group].bg_inode_bitmap);
    }
    else if (fs->inode_bitmap_id != fs->bgdt[group].bg_inode_bitmap) {
        block_munmap(fs, fs->last_inode_bitmap);
        fs->inode_bitmap_id = fs->bgdt[group].bg_inode_bitmap;
        fs->last_inode_bitmap = block_mmap(fs, fs->bgdt[group].bg_inode_bitmap);
    }

    __u8 ans = !!(*(__u8*)(fs->last_inode_bitmap + id / 8) & (1 << id % 8));
    return ans;
}

__u32 put_inode(struct ext2_file_system *me, struct inode inode_, __u32 inode_dir_id) {
    return 0;
}

__u32 delete_inode(struct ext2_file_system *me, __u32 inode) {
    return 0;
}