#include "ext2_fs.h"

__u32 ext2_file_system_create(const char *file) {
    return 0;
}

__u32 ext2_file_system_init(struct ext2_file_system *fs, const char *file) {
    #ifdef __unix__
        __u32 fd = open(file, O_RDWR);

        __u32 err;
        void *ptr = mmap(NULL, 2048, PROT_READ, MAP_PRIVATE, fd, 0);
        if (ptr == MAP_FAILED) {
            return -2;
        }

        fs->fd = fd;
        fs->sb = *(struct super_block*)(ptr + 1024);

        err = munmap(ptr, 1024);
        if (err < 0) {
            return -3;
        }

        fs->block_size = get_block_size(&fs->sb);
        fs->groups_count = CEIL_DIV(fs->sb.s_blocks_count, fs->sb.s_blocks_per_group);

        __u32 sz = fs->groups_count * sizeof(struct block_group_descriptor);
        fs->bgdt = malloc(sz);
        ptr = mmap(NULL, sz, PROT_READ, MAP_PRIVATE, fd, (1 + fs->sb.s_first_data_block) * fs->block_size); // THIS MAY CAUSE MAP_FAILED!!!!!
        if (ptr == MAP_FAILED) {
            return -4;
        }

        struct block_group_descriptor *tmp_ptr = ptr;
        __u32 i;
        for (i = 0; i < fs->groups_count; i++, tmp_ptr++) {
            fs->bgdt[i] = *tmp_ptr;
        }

        err = munmap(ptr, sz);
        if (err < 0) {
            return -5;
        }


    #elif defined _WIN32
        fs->file = CreateFileA(file,
            GENERIC_READ | GENERIC_WRITE,
            FILE_SHARE_READ | FILE_SHARE_WRITE,
            NULL,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL,
            NULL);
        
        fs->mapping = CreateFileMappingA(fs->file,
            NULL,
            PAGE_READWRITE,
            0, 0,
            NULL);


            
        void *ptr = MapViewOfFile(fs->mapping,
            FILE_MAP_READ | FILE_MAP_WRITE,
            0, 0,
            2048);
        
        if (ptr == NULL) {
            return -2;
        }

        fs->sb = *(struct super_block*)((char*)ptr + 1024);

        UnmapViewOfFile(ptr);

        fs->block_size = get_block_size(&fs->sb);
        fs->groups_count = CEIL_DIV(fs->sb.s_blocks_count, fs->sb.s_blocks_per_group);

        __u32 sz = fs->groups_count * sizeof(struct block_group_descriptor);
        fs->bgdt = malloc(sz);
        ptr = MapViewOfFile(fs->mapping,
            FILE_MAP_READ | FILE_MAP_WRITE,
            0, 0,
            (1 + fs->sb.s_first_data_block) * fs->block_size + sz);
        if (ptr == NULL) {
            return -4;
        }

        struct block_group_descriptor *tmp_ptr = (struct block_group_descriptor*)((char*)ptr + (1 + fs->sb.s_first_data_block) * fs->block_size);
        __u32 i;
        for (i = 0; i < fs->groups_count; i++, tmp_ptr++) {
            fs->bgdt[i] = *tmp_ptr;
        }

        UnmapViewOfFile(ptr);
    #endif

    fs->block_bitmap_id = 0;
    fs->last_block_bitmap = NULL;
    fs->inode_bitmap_id = 0;
    fs->last_inode_bitmap = NULL;
    return 0;
}

static void save_super_block_and_bgdt_at_address(struct ext2_file_system *fs, void *ptr, __u16 id) {
    *(struct super_block*)ptr = fs->sb;
    ((struct super_block*)ptr)->s_block_group_nr = id;
    struct block_group_descriptor *bgdt_ptr = (struct block_group_descriptor*)((char*)ptr + get_block_size_from_fs(fs));
    int i;
    for (i = 0; i < fs->groups_count; i++) {
        bgdt_ptr[i] = fs->bgdt[i];
    }
}

static void save_super_block_and_bgdt_at_powers_of(struct ext2_file_system *fs, int power) {
    __u32 bytes_in_group = get_block_size_from_fs(fs) * fs->sb.s_blocks_per_group;
    int i;
    void *ptr;
    __u32 size_in_bytes = get_block_size_from_fs(fs) + fs->groups_count * sizeof(struct block_group_descriptor);
    for (i = power; i < fs->groups_count; i *= power) {
        #ifdef __unix__
            ptr = mmap(NULL, size_in_bytes, PROT_WRITE | PROT_READ, MAP_SHARED, fs->fd, bytes_in_group * i);
            save_super_block_and_bgdt_at_address(fs, ptr, i);
            munmap(ptr, size_in_bytes);
        #elif defined _WIN32
            // The offset must be a multiple of the VirtualAlloc allocation granularity.
            SYSTEM_INFO si;
            GetSystemInfo(&si);
            SIZE_T gran = si.dwAllocationGranularity;

            size_t real_offset = bytes_in_group * i;

            size_t offset_in_gran = (real_offset / gran) * gran;
            size_t gran_garbage = real_offset - offset_in_gran;
            ptr = MapViewOfFile(fs->mapping,
                FILE_MAP_READ | FILE_MAP_WRITE,
                0, offset_in_gran,
                gran_garbage + size_in_bytes);
            save_super_block_and_bgdt_at_address(fs, (char*)ptr + gran_garbage, i);
            UnmapViewOfFile(ptr);
        #endif
    }
}
// sparse super_block
static void save_super_block_and_bgdt_at_chosen_groups(struct ext2_file_system *fs) {
    void *ptr;
    __u32 bytes_in_group = get_block_size_from_fs(fs) * fs->sb.s_blocks_per_group;
    __u32 size_in_bytes = get_block_size_from_fs(fs) + fs->groups_count * sizeof(struct block_group_descriptor);
    if (fs->groups_count > 0) {
        #ifdef __unix__
            ptr = mmap(NULL, 2048, PROT_READ | PROT_WRITE, MAP_SHARED, fs->fd, 0);
            *(struct super_block*)ptr = fs->sb;
            munmap(ptr, 2048);

            ptr = mmap(NULL, bytes_in_group, PROT_READ | PROT_WRITE, MAP_SHARED, fs->fd, (1 + fs->sb.s_first_data_block) * fs->block_size);
            __u32 i;
            struct block_group_descriptor *tmp_ptr = ptr;
            for (i = 0; i < fs->groups_count; i++) {
                tmp_ptr[i] = fs->bgdt[i];
            }
            munmap(ptr, bytes_in_group);
        #elif defined _WIN32
            ptr = MapViewOfFile(fs->mapping,
                FILE_MAP_READ | FILE_MAP_WRITE,
                0, 0,
                2048);
            *(struct super_block*)ptr = fs->sb;
            UnmapViewOfFile(ptr);

            ptr = MapViewOfFile(fs->mapping,
                FILE_MAP_READ | FILE_MAP_WRITE,
                0, 0,
                (1 + fs->sb.s_first_data_block) * fs->block_size + bytes_in_group);
            __u32 i;
            struct block_group_descriptor *tmp_ptr = (struct block_group_descriptor*)((char*)ptr + (1 + fs->sb.s_first_data_block) * fs->block_size);
            for (i = 0; i < fs->groups_count; i++) {
                tmp_ptr[i] = fs->bgdt[i];
            }
            UnmapViewOfFile(ptr);
        #endif
    }
    if (fs->groups_count > 1) {
        #ifdef __unix__
            ptr = mmap(NULL, size_in_bytes, PROT_READ | PROT_WRITE, MAP_SHARED, fs->fd, bytes_in_group);
            save_super_block_and_bgdt_at_address(fs, ptr, 1);
            munmap(ptr, size_in_bytes);
        #elif defined _WIN32
            SYSTEM_INFO si;
            GetSystemInfo(&si);
            SIZE_T gran = si.dwAllocationGranularity;

            size_t real_offset = bytes_in_group;

            size_t offset_in_gran = (real_offset / gran) * gran;
            size_t gran_garbage = real_offset - offset_in_gran;
            ptr = MapViewOfFile(fs->mapping,
                FILE_MAP_READ | FILE_MAP_WRITE,
                0, offset_in_gran,
                gran_garbage + size_in_bytes);
            save_super_block_and_bgdt_at_address(fs, (char*)ptr + gran_garbage, 1);
            UnmapViewOfFile(ptr);
        #endif
    }
    save_super_block_and_bgdt_at_powers_of(fs, 3);
    save_super_block_and_bgdt_at_powers_of(fs, 5);
    save_super_block_and_bgdt_at_powers_of(fs, 7);
}

__u32 ext2_file_system_destroy(struct ext2_file_system *fs) {
    save_super_block_and_bgdt_at_chosen_groups(fs);
    if (fs->last_block_bitmap != 0) { block_munmap(fs, fs->last_block_bitmap); }
    if (fs->last_inode_bitmap != 0) { block_munmap(fs, fs->last_inode_bitmap); }
    free(fs->bgdt);
    #ifdef __unix__
        close(fs->fd);
    #elif defined _WIN32
        CloseHandle(fs->mapping);
        fs->mapping = NULL;

        CloseHandle(fs->file);
        fs->file = NULL;
    #endif
    return 0;
}

struct super_block get_super_block(struct ext2_file_system *fs) {
    return fs->sb;
}
__u32 set_super_block(struct ext2_file_system *fs, struct super_block sb) {
    fs->sb = sb;
    return 0;
}

__u32 get_block_size_from_fs(struct ext2_file_system *fs) {
    return fs->block_size;
}

struct block_group_descriptor get_bgd(struct ext2_file_system *fs, __u32 index) {
    return fs->bgdt[index];
}
__u32 set_bgd(struct ext2_file_system *fs, struct block_group_descriptor bgd,__u32 index) {
    fs->bgdt[index] = bgd;
    return 0;
}

// please be aware that it can fail because of a page size
// it doesn't fail for 4K block size
void* block_mmap(struct ext2_file_system *fs, __u32 id) {
    void *ptr = NULL;
    #ifdef __unix__
        ptr = mmap(NULL, fs->block_size, PROT_READ | PROT_WRITE, MAP_SHARED, fs->fd, fs->block_size * id);
        if (ptr == MAP_FAILED) {
            printf("BLOCK_MMAP FAILURE!!! BLOCK ID: %u AND BLOCK SIZE: %u", id, fs->block_size);
            return NULL;
        }
    #elif defined _WIN32
        SYSTEM_INFO si;
        GetSystemInfo(&si);
        SIZE_T gran = si.dwAllocationGranularity;

        size_t real_offset = fs->block_size * id;

        size_t offset_in_gran = (real_offset / gran) * gran;
        size_t gran_garbage = real_offset - offset_in_gran;
        ptr = MapViewOfFile(fs->mapping,
            FILE_MAP_READ | FILE_MAP_WRITE,
            0, offset_in_gran,
            gran_garbage + fs->block_size);
        ptr = (char*)ptr + gran_garbage;
    #endif
    return ptr;
}

__u32 block_munmap(struct ext2_file_system *fs, void *ptr) {
    #ifdef __unix__
        return munmap(ptr, fs->block_size);
    #elif defined _WIN32
    // This value must be identical to the value returned by a previous call to one of the functions in the MapViewOfFile family
    // therefore the pointer value which was created after block_mmap call must be found
    // this value can be found with calcutaling the multiple of granularity of pointer
        SYSTEM_INFO si;
        GetSystemInfo(&si);
        SIZE_T gran = si.dwAllocationGranularity;
        size_t pointer_value = (char*)ptr - (char*)NULL;
        size_t garbage = pointer_value - (pointer_value / gran) * gran;
        return UnmapViewOfFile((void*)((char*)ptr - garbage));
    #endif
}



void check_and_change_block_bitmap(struct ext2_file_system *fs, __u32 group) {
    if (fs->last_block_bitmap == NULL) {
        fs->block_bitmap_id = fs->bgdt[group].bg_block_bitmap;
        fs->last_block_bitmap = block_mmap(fs, fs->bgdt[group].bg_block_bitmap);
    }
    else if (fs->block_bitmap_id != fs->bgdt[group].bg_block_bitmap) {
        block_munmap(fs, fs->last_block_bitmap);
        fs->block_bitmap_id = fs->bgdt[group].bg_block_bitmap;
        fs->last_block_bitmap = block_mmap(fs, fs->bgdt[group].bg_block_bitmap);
    }
}

void check_and_change_inode_bitmap(struct ext2_file_system *fs, __u32 group) {
    if (fs->last_inode_bitmap == NULL) {
        fs->inode_bitmap_id = fs->bgdt[group].bg_inode_bitmap;
        fs->last_inode_bitmap = block_mmap(fs, fs->bgdt[group].bg_inode_bitmap);
    }
    else if (fs->inode_bitmap_id != fs->bgdt[group].bg_inode_bitmap) {
        block_munmap(fs, fs->last_inode_bitmap);
        fs->inode_bitmap_id = fs->bgdt[group].bg_inode_bitmap;
        fs->last_inode_bitmap = block_mmap(fs, fs->bgdt[group].bg_inode_bitmap);
    }
}

__u8 is_block_used(struct ext2_file_system *fs, __u32 id) {
    __u32 group = id / fs->sb.s_blocks_per_group;
    id = id % fs->sb.s_blocks_per_group;
    
    check_and_change_block_bitmap(fs, group);
     
    __u8 ans = !!(*(__u8*)((char*)fs->last_block_bitmap + id / 8) & (1 << id % 8));
    return ans;
}

__u8 is_inode_used(struct ext2_file_system *fs, __u32 id) {
    __u32 group = (id - 1) / fs->sb.s_inodes_per_group;
    id = (id - 1) % fs->sb.s_inodes_per_group;
   
    check_and_change_inode_bitmap(fs, group);

    __u8 ans = !!(*(__u8*)((char*)fs->last_inode_bitmap + id / 8) & (1 << id % 8));
    return ans;
}

__u8 set_bit_block_bitmap(struct ext2_file_system *fs, __u32 id) {
    __u32 group = id / fs->sb.s_blocks_per_group;
    id = id % fs->sb.s_blocks_per_group;

    check_and_change_block_bitmap(fs, group);

    *(__u8*)((char*)fs->last_block_bitmap + id / 8) |= (1 << id % 8);
    return 0;
}

__u8 set_bit_inode_bitmap(struct ext2_file_system *fs, __u32 id) {
    __u32 group = (id - 1) / fs->sb.s_inodes_per_group;
    id = (id - 1) % fs->sb.s_inodes_per_group;
   
    check_and_change_inode_bitmap(fs, group);

    *(__u8*)((char*)fs->last_inode_bitmap + id / 8) |= (1 << id % 8);
    return 0;
}

__u8 unset_bit_block_bitmap(struct ext2_file_system *fs, __u32 id) {
    __u32 group = id / fs->sb.s_blocks_per_group;
    id = id % fs->sb.s_blocks_per_group;
   
    check_and_change_block_bitmap(fs, group);

    *(__u8*)((char*)fs->last_block_bitmap + id / 8) &= ~(1 << id % 8);
    return 0;
}
__u8 unset_bit_inode_bitmap(struct ext2_file_system *fs, __u32 id) {
    __u32 group = (id - 1) / fs->sb.s_inodes_per_group;
    id = (id - 1) % fs->sb.s_inodes_per_group;
   
    check_and_change_inode_bitmap(fs, group);

    *(__u8*)((char*)fs->last_inode_bitmap + id / 8) &= ~(1 << id % 8);
    return 0;
}

__u32 block_alloc(struct ext2_file_system *fs, __u32 block) {
    if (is_block_used(fs, block)) {
        return -1;
    }
    fs->sb.s_free_blocks_count--;
    fs->bgdt[block / fs->sb.s_blocks_per_group].bg_free_blocks_count--;
    set_bit_block_bitmap(fs, block);
    return 0;
}
__u32 free_block(struct ext2_file_system *fs, __u32 block) {
    if (!is_block_used(fs, block)) {
        return -1;
    }
    fs->sb.s_free_blocks_count++;
    fs->bgdt[block / fs->sb.s_blocks_per_group].bg_free_blocks_count++;
    unset_bit_block_bitmap(fs, block);
    return 0;
}

/* this is a really tricky function.
* should i also make it preallocate
* blocks in future?
*/
__u32 first_free_block(struct ext2_file_system *fs, __u32 hint) {
    __u32 id;
    /* it should try to find new blocks within 64 blocks of the ideal hint-block
    */
    for (id = hint; id < fs->sb.s_blocks_count; id++) {
        if(!is_block_used(fs, id)) {
            return id;
        }
    }
    for (id = 0; id < hint; id++) {
        if(!is_block_used(fs, id)) {
            return id;
        }
    }
    return 0;
}

__u32 first_free_inode(struct ext2_file_system *fs, __u32 hint) {
    __u32 id;
    for (id = hint; id < fs->sb.s_inodes_count; id++) {
        if (!is_inode_used(fs, id)) {
            return id;
        }
    }
    for (id = 1; id < hint; id++) {
        if (!is_inode_used(fs, id)) {
            return id;
        }
    }
    return 0;
}

struct inode read_inode(struct ext2_file_system *fs, __u32 inode) {
    __u32 block_group_index = (inode - 1) / fs->sb.s_inodes_per_group;
    __u32 local_inode_index = (inode - 1) % fs->sb.s_inodes_per_group;
    __u32 id = fs->bgdt[block_group_index].bg_inode_table + local_inode_index / (fs->block_size / fs->sb.s_inode_size);
    void *ptr = block_mmap(fs, id);
    struct inode temp = *(struct inode*)((char*)ptr + fs->sb.s_inode_size * (local_inode_index % (fs->block_size / fs->sb.s_inode_size)));
    block_munmap(fs, ptr);
    return temp;
}

__u32 inode_alloc(struct ext2_file_system *fs, __u32 inode) {
    if (is_inode_used(fs, inode)) {
        return -1;
    }
    fs->sb.s_free_inodes_count--;
    fs->bgdt[inode / fs->sb.s_inodes_per_group].bg_free_inodes_count--;
    set_bit_inode_bitmap(fs, inode);
    return 0;
}
__u32 free_inode(struct ext2_file_system *fs, __u32 inode) {
    if (!is_inode_used(fs, inode)) {
        return -1;
    }
    fs->sb.s_free_inodes_count++;
    fs->bgdt[inode / fs->sb.s_inodes_per_group].bg_free_inodes_count++;
    unset_bit_inode_bitmap(fs, inode);
    return 0;
}

__u32 put_inode(struct ext2_file_system *fs, struct inode inode_, __u32 inode_id) {
    __u32 block_group_index = (inode_id - 1) / fs->sb.s_inodes_per_group;
    __u32 local_inode_index = (inode_id - 1) % fs->sb.s_inodes_per_group;
    __u32 id = fs->bgdt[block_group_index].bg_inode_table + local_inode_index / (fs->block_size / fs->sb.s_inode_size);
    void *ptr = block_mmap(fs, id);
    struct inode *temp = (struct inode*)((char*)ptr + fs->sb.s_inode_size * (local_inode_index % (fs->block_size / fs->sb.s_inode_size)));
    *temp = inode_;
    block_munmap(fs, ptr);
    return 0;
}

__u32 get_block_id_occupied_by_inode(struct ext2_file_system *fs, __u32 inode_id) {
    __u32 block_group_index = (inode_id - 1) / fs->sb.s_inodes_per_group;
    __u32 local_inode_index = (inode_id - 1) % fs->sb.s_inodes_per_group;
    __u32 id = fs->bgdt[block_group_index].bg_inode_table + local_inode_index / (fs->block_size / fs->sb.s_inode_size);
    return id;
}
