#!/bin/bash
# Setup fscrypt for directory encryption

set -e

DIR="${1:-/mnt/encrypted_dir}"

echo "Setting up fscrypt for: $DIR"

# Install fscrypt if needed
if ! command -v fscrypt >/dev/null 2>&1; then
    echo "fscrypt not installed. Install with: apt-get install fscrypt"
    exit 1
fi

# Create directory
mkdir -p "$DIR"

# Setup encryption policy
fscrypt setup "$DIR"

# Create encrypted directory
fscrypt encrypt "$DIR"

echo "fscrypt setup complete for: $DIR"

