#include "file_desc.h"

__u32 fd_init(struct file_desc *fd, struct ext2_file_system *fs, struct inode *inode, __u32 inode_id, __u32 flags) {
    inode_gate_init(&fd->ig, fs, inode);

    fd->inode_id = inode_id;
    fd->cur_offset = 0;
    fd->flags = flags;

    fd->block_size = get_block_size_from_fs(fs);
}
__u32 fd_destroy(struct file_desc *fd) {
    inode_gate_destroy(&fd->ig);
}

__u32 fd_move_by_offset(struct file_desc *fd, __u32 offset) {
    if (offset == 0) {
        return 0;
    }
    if (fd->cur_offset + offset > fd->ig.inode->i_size) {
        return 1;
    }

    __u32 bytes_until_next_block = fd->block_size - fd->cur_offset % fd->block_size;
    __u32 blocks = 0;
    if (bytes_until_next_block <= offset) {
        blocks = 1 + (offset - bytes_until_next_block) / fd->block_size;
    }
    move_by_offset(&fd->ig, blocks);
    fd->cur_offset += offset;
    return 0;
}

__u32 fd_read(struct file_desc *fd, __u32 size, void *buf) {
    if (fd->cur_offset + size > fd->ig.inode->i_size) {
        size = fd->ig.inode->i_size - fd->cur_offset;
    }
    __u32 bytes_read = 0;

    while (bytes_read < size) {
        __u32 offset_in_block = fd->cur_offset % fd->block_size;

        __u32 bytes_to_copy = fd->block_size - offset_in_block;
        if (bytes_to_copy > size - bytes_read) {
            bytes_to_copy = size - bytes_read;
        }

        __u32 current_block_id = get_current_block_id(&fd->ig);
        if (current_block_id == 0) { return 0; }

        void* ptr = block_mmap(fd->ig.fs, current_block_id);

        memcpy(buf + bytes_read, ptr + offset_in_block, bytes_to_copy);

        block_munmap(fd->ig.fs, ptr);

        bytes_read += bytes_to_copy;

        if (fd_move_by_offset(fd, bytes_to_copy) != 0) { return 0; }
    }
    return bytes_read;
}
__u32 fd_write(struct file_desc *fd, __u32 size, void *buf) {
    __u32 bytes_written = 0;

    while (bytes_written < size) {
        __u32 offset_in_block = fd->cur_offset % fd->block_size;

        __u32 bytes_to_write = fd->block_size - offset_in_block;
        if (bytes_to_write > size - bytes_written) {
            bytes_to_write = size - bytes_written;
        }

        __u32 logical_block = fd->cur_offset / fd->block_size;
        __u32 allocated_blocks = get_real_size_in_alloc_blocks(&fd->ig);

        if (logical_block >= allocated_blocks) {
            __u32 new_block_id = first_free_block(fd->ig.fs, get_current_block_id(&fd->ig));
            block_alloc(fd->ig.fs, new_block_id);
            append_block(&fd->ig, new_block_id);
            next_block(&fd->ig);
        }

        __u32 block_id = get_current_block_id(&fd->ig);

        void *ptr = block_mmap(fd->ig.fs, block_id);

        memcpy(ptr + offset_in_block, buf + bytes_written, bytes_to_write);

        block_munmap(fd->ig.fs, ptr);

        bytes_written += bytes_to_write;

        fd_move_by_offset(fd, bytes_to_write);
    }

    if (fd->cur_offset > fd->ig.inode->i_size) {
        fd->ig.inode->i_size = fd->cur_offset;
    }

    return bytes_written;
}

__u32 fd_ftruncate(struct file_desc *fd, __u32 length) {
    if (fd->ig.inode->i_size == length) { return 0; }
    if (fd->ig.inode->i_size < length) {
        __u32 logical_block = length / fd->block_size;
        __u32 allocated_blocks = get_real_size_in_alloc_blocks(&fd->ig);
        while (logical_block >= allocated_blocks) {
            __u32 new_block_id = first_free_block(fd->ig.fs, get_current_block_id(&fd->ig));
            block_alloc(fd->ig.fs, new_block_id);
            append_block(&fd->ig, new_block_id);

            allocated_blocks = get_real_size_in_alloc_blocks(&fd->ig);
        }
    }
    else {
        __u32 logical_block = length / fd->block_size;
        __u32 allocated_blocks = get_real_size_in_alloc_blocks(&fd->ig);
        while (logical_block + 1 < allocated_blocks) {
            __u32 block_id = unlink_last_block(&fd->ig);
            free_block(fd->ig.fs, block_id);

            allocated_blocks = get_real_size_in_alloc_blocks(&fd->ig);
        }
    }

    fd->ig.inode->i_size = length;
    return 0;
}