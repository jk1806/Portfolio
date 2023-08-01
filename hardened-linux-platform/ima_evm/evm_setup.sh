#!/bin/bash
# Setup EVM (Extended Verification Module)

set -e

echo "Setting up EVM..."

# Check if EVM is enabled
if [ ! -d /sys/kernel/security/evm ]; then
    echo "EVM not available in kernel"
    exit 1
fi

# Generate EVM key
if [ ! -f /etc/keys/evm_key.bin ]; then
    mkdir -p /etc/keys
    dd if=/dev/urandom of=/etc/keys/evm_key.bin bs=32 count=1
    chmod 600 /etc/keys/evm_key.bin
fi

# Load EVM key
echo "Loading EVM key..."
keyctl add user evm-key "$(cat /etc/keys/evm_key.bin)" @u

# Enable EVM
echo 1 > /sys/kernel/security/evm

echo "EVM setup complete"

