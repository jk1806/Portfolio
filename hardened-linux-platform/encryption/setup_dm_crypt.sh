#!/bin/bash
# Setup dm-crypt for disk encryption

set -e

DEVICE="/dev/sdb1"
MAPPER_NAME="encrypted_data"
KEYFILE="/etc/keys/dmcrypt.key"

echo "Setting up dm-crypt encryption..."

# Generate key if not exists
if [ ! -f "$KEYFILE" ]; then
    mkdir -p $(dirname $KEYFILE)
    dd if=/dev/urandom of=$KEYFILE bs=512 count=8
    chmod 600 $KEYFILE
fi

# Create encrypted device
cryptsetup luksFormat $DEVICE $KEYFILE
cryptsetup luksOpen $DEVICE $MAPPER_NAME --key-file $KEYFILE

# Create filesystem
mkfs.ext4 /dev/mapper/$MAPPER_NAME

# Mount
mkdir -p /mnt/encrypted
mount /dev/mapper/$MAPPER_NAME /mnt/encrypted

echo "Encrypted device mounted at /mnt/encrypted"

