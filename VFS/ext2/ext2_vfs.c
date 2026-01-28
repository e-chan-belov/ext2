#include "ext2_vfs.h"

static __u32 get_current_posix_time() {
    return (__u32)time(NULL);
}

static __u32 alloc_new_inode(struct ext2_vfs *vfs, __u32 hint) {
    __u32 inode_id = first_free_inode(vfs->fs, hint);
    inode_alloc(vfs->fs, inode_id);
    return inode_id;
}

static struct inode create_default_file(__u16 user_id, __u16 group_id, __u16 file_type) {
    struct inode inode;

    __u16 process_bits = 0;
    __u16 default_permission = EXT2_S_IRUSR | EXT2_S_IWUSR | EXT2_S_IRGRP | EXT2_S_IWGRP | EXT2_S_IROTH;
    inode.i_mode = file_type | process_bits | default_permission;

    inode.i_uid = user_id;
    inode.i_gid = group_id;

    __u32 now = get_current_posix_time();
    inode.i_atime = now;
    inode.i_ctime = now;
    inode.i_mtime = now;
    inode.i_dtime = 0;

    inode.i_flags = 0;

    inode.i_size = 0;
    inode.i_blocks = 0;
    int i;
    for (i = 0; i < 15; i++) {
        inode.i_block[i] = 0;
    }

    inode.i_links_count = 0;

    inode.i_osd1 = 2;

    inode.i_generation = 3948355924;
    inode.i_file_acl = 0;
    inode.i_dir_acl = 0;
    inode.i_faddr = 0;
    for (i = 0; i < 12; i++) {
        inode.i_osd2[i] = 0;
    }

    return inode;
}

static void create_default_dir(struct ext2_vfs *vfs, __u32 parent_inode, const char* name) {
    __u32 inode_id = alloc_new_inode(vfs, parent_inode);
    struct inode inode = create_default_file(vfs->user_id, vfs->group_id, EXT2_S_IFDIR);
    put_inode(vfs->fs, inode, inode_id);
    add_new_entry(vfs->fs, parent_inode, name, DIR_TYPE_DIR, inode_id);

    add_new_entry(vfs->fs, inode_id, ".", DIR_TYPE_DIR, inode_id);
    add_new_entry(vfs->fs, inode_id, "..", DIR_TYPE_DIR, parent_inode);
    
    // this is temporary
    struct inode root_dir = read_inode(vfs->fs, parent_inode);
    root_dir.i_links_count--;
    put_inode(vfs->fs, root_dir, parent_inode);
}

static __u8 dir_entry_name_compare_with(struct ext2_dir_entry *dentry, const char *name) {
    __u16 search_len = (__u16)strlen(name);
    if (search_len != dentry->name_len) {
        return 0;
    }

    const char *dentry_name = ext2_dir_entry_get_name(dentry);

    return strncmp(name, dentry_name, search_len) == 0;
}

static find_inode_id_by_name_in_dir(struct ext2_vfs *vfs, const char *name, __u32 dir_id) {
    struct dir_gate dg;
    __u32 error;
    error = dir_gate_init(&dg, vfs->fs, dir_id);
    if (error == 1) { return 0; }

    struct ext2_dir_entry entry;
    int dir_gate_error = 0;
    for (; dir_gate_error != 1; dir_gate_error = next_entry(&dg)) {
        ext2_dir_entry_init(&entry, dg.current_block + dg.offset);
        if (dir_entry_name_compare_with(&entry, name)) {
            return entry.inode;
        }
    }
    return 0;
}

const char* get_symlink_path(struct ext2_vfs *vfs, __u32 id) {
    struct inode symlink_inode = read_inode(vfs->fs, id);
    char *str = malloc(symlink_inode.i_size * sizeof(char));
    int i = 0;
    if (symlink_inode.i_size < 60) {
        char *ptr = (char*)symlink_inode.i_block;
        for (; i < symlink_inode.i_size; i++, ptr++) {
            str[i] = *ptr;
        }
        str[i] = 0;
    } else {
        struct inode_gate ig;
        inode_gate_init(&ig, vfs->fs, &symlink_inode);

        char *ptr = (char*)block_mmap(vfs->fs, get_current_block_id(&ig));
        for (; i < symlink_inode.i_size; i++, ptr++) {
            str[i] = *ptr;
        }
        str[i] = 0;

        inode_gate_destroy(&ig);
    }
    return str;
}

// todo: add symlink support
static __u32 find_inode_id_by_path(struct ext2_vfs *vfs, const char *path) {
    char *path_copy = strdup(path);

    __u32 parent_inode = ROOT_DIR_INODE_ID;
    __u32 temp = 0;
    char *token = strtok(path_copy, "/");
    while (token != NULL) {
        struct inode cur_inode = read_inode(vfs->fs, parent_inode);
        if (IS_LNK(cur_inode.i_mode)) {
            const char *sym_path = get_symlink_path(vfs, parent_inode);
            temp = find_inode_id_by_path(vfs, sym_path);
            if (temp == 0) { 
                free(sym_path);
                free(path_copy);
                return 0; 
            }
            free(sym_path);
        }
        else if (IS_DIR(cur_inode.i_mode)) {
            temp = find_inode_id_by_name_in_dir(vfs, token, parent_inode);
            if (temp == 0) { 
                free(path_copy);
                return 0; 
            }
        }
        else {
            free(path_copy);
            return 0; 
        }

        parent_inode = temp;
        token = strtok(NULL, "/");
    }

    free(path_copy);
    return parent_inode;
}

__s32 ext2_vfs_mkdir(struct ext2_vfs *vfs, const char *path, const char *name) {
    __u32 dir_id = find_inode_id_by_path(vfs, path);
    if (dir_id == 0) { return -1; }

    __u32 already_exist_id = find_inode_id_by_name_in_dir(vfs, name, dir_id);
    if (already_exist_id != 0) { return -2; }

    create_default_dir(vfs, dir_id, name);
    return 0;
}

__s32 ext2_vfs_touch(struct ext2_vfs *vfs, __u32 option, const char *path, const char *name) {
    __u32 dir_id = find_inode_id_by_path(vfs, path);
    if (dir_id == 0) { return -1; }

    __u32 file_id = find_inode_id_by_name_in_dir(vfs, name, dir_id);
    if (file_id == 0 && option != 2) {
        struct inode file = create_default_file(vfs->user_id, vfs->group_id, EXT2_S_IFREG);

        file_id = first_free_inode(vfs->fs, ROOT_DIR_INODE_ID);
        inode_alloc(vfs->fs, file_id);
        put_inode(vfs->fs, file, file_id);

        add_new_entry(vfs->fs, dir_id, name, DIR_TYPE_FILE, file_id);
        return 0;
    }
    struct inode file = read_inode(vfs->fs, file_id);

    __u32 current_time = get_current_posix_time();

    switch (option)
    {
    case 0:
        file.i_atime = current_time;
        file.i_mtime = current_time;
        break;
    
    case 1:
        break;
    }
    put_inode(vfs->fs, file, file_id);
    return 0;
}

__s32 ext2_vfs_list(struct ext2_vfs *vfs, const char *path) {
    __u32 dir_id = find_inode_id_by_path(vfs, path);
    if (dir_id == 0) { return -1; }

    struct dir_gate dg;
    __u32 dir_gate_error = dir_gate_init(&dg, vfs->fs, dir_id);
    if (dir_gate_error != 0) { return -2; }

    dir_gate_error = 0;
    struct ext2_dir_entry entry;
    for (; dir_gate_error != 1; dir_gate_error = next_entry(&dg)) {
        entry = get_current_entry(&dg);
        printf("%s ", ext2_dir_entry_get_name(&entry));
    }
    printf("\n");

    dir_gate_destroy(&dg);
    return 0;
}

__s32 ext2_vfs_unlink(struct ext2_vfs *vfs, const char *path, const char *name) {
    __u32 dir_id = find_inode_id_by_path(vfs, path);
    if (dir_id == 0) { return -1; }

    struct dir_gate dg;
    __u32 dir_gate_error = dir_gate_init(&dg, vfs->fs, dir_id);
    if (dir_gate_error != 0) { return -2; }

    dir_gate_error = 0;
    struct ext2_dir_entry entry;
    for (; dir_gate_error != 1; dir_gate_error = next_entry(&dg)) {
        entry = get_current_entry(&dg);
        if (dir_entry_name_compare_with(&entry, name)) {
            delete_current_entry(&dg);
            dir_gate_destroy(&dg);
            return 0;
        }
    }

    dir_gate_destroy(&dg);
    return -3;
}