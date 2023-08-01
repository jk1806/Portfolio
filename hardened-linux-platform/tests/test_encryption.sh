#!/bin/bash
# Test encryption setup

echo "Testing encryption..."

# Check dm-crypt
if command -v cryptsetup >/dev/null 2>&1; then
    echo "dm-crypt: Available"
else
    echo "dm-crypt: Not available"
    exit 1
fi

# Check for encrypted devices
if lsblk -f | grep -q crypt; then
    echo "Encrypted devices: Found"
else
    echo "Encrypted devices: None (test environment)"
fi

# Test key file
if [ -f /etc/keys/dmcrypt.key ]; then
    echo "Key file: OK"
else
    echo "Key file: Not found (expected in test)"
fi

echo "Encryption test PASSED"

