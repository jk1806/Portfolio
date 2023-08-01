#!/bin/bash
# Setup SMACK

set -e

echo "Setting up SMACK..."

# Check if SMACK is enabled in kernel
if [ ! -d /sys/fs/smackfs ]; then
    echo "SMACK not available in kernel"
    echo "Enable with: CONFIG_SECURITY_SMACK=y"
    exit 1
fi

# Load SMACK policy
cat selinux/policies/smack_policy > /sys/fs/smackfs/load2

# Set default labels
echo "SystemFiles" > /sys/fs/smackfs/ambient
echo "EmbeddedApp" > /sys/fs/smackfs/onlycap

echo "SMACK setup complete"

