#!/bin/bash
# QEMU simulation script for secure boot

QEMU=qemu-system-arm
KERNEL=zImage
DTB=vexpress-v2p-ca9.dtb
ROOTFS=rootfs.cpio

echo "Starting QEMU simulation..."

$QEMU -M vexpress-a9 \
    -m 512M \
    -kernel $KERNEL \
    -dtb $DTB \
    -initrd $ROOTFS \
    -append "console=ttyAMA0 root=/dev/ram" \
    -nographic

