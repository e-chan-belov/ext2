#!/usr/bin/env bash
set -eux

DISK_FILE=/workspaces/ext2_test_first/disk.img
MOUNT_POINT=/mnt/virtual_disk

if [ ! -f "$DISK_FILE" ]; then
    echo "Creating disk image..."
    dd if=/dev/zero of=$DISK_FILE bs=1M count=200
    mkfs.ext2 -F $DISK_FILE
fi

mkdir -p $MOUNT_POINT

mount -o loop $DISK_FILE $MOUNT_POINT

echo "Disk mounted at $MOUNT_POINT"