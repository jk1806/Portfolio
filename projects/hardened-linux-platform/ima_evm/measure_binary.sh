#!/bin/bash
# Measure binary with IMA

BINARY="$1"

if [ -z "$BINARY" ]; then
    echo "Usage: $0 <binary>"
    exit 1
fi

if [ ! -f "$BINARY" ]; then
    echo "Error: Binary not found: $BINARY"
    exit 1
fi

echo "Measuring binary: $BINARY"

# Calculate SHA256 hash
HASH=$(sha256sum "$BINARY" | cut -d' ' -f1)

# Store measurement
echo "$HASH $BINARY" >> /sys/kernel/security/ima/ascii_runtime_measurements

echo "Binary measured: $HASH"

