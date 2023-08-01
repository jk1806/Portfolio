#!/bin/bash
# Setup mount namespace

MOUNTNS_NAME="embedded_mountns"
MOUNT_POINT="/mnt/isolated"

# Create mount namespace
unshare --mount --fork bash -c "
    # Mount tmpfs
    mount -t tmpfs tmpfs $MOUNT_POINT
    
    # Create isolated filesystem
    mkdir -p $MOUNT_POINT/{bin,lib,etc}
    
    echo 'Mount namespace created: $MOUNTNS_NAME'
    echo 'Isolated mount point: $MOUNT_POINT'
    
    # Keep namespace alive
    exec bash
"

