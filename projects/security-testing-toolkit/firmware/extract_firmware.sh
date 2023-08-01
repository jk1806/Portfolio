#!/bin/bash
# Extract firmware from device or file

set -e

FIRMWARE_FILE="${1:-firmware.bin}"
OUTPUT_DIR="extracted"

if [ ! -f "$FIRMWARE_FILE" ]; then
    echo "Error: Firmware file not found: $FIRMWARE_FILE"
    exit 1
fi

echo "Extracting firmware: $FIRMWARE_FILE"

mkdir -p "$OUTPUT_DIR"

# Use binwalk to extract
if command -v binwalk >/dev/null 2>&1; then
    binwalk -e "$FIRMWARE_FILE" -C "$OUTPUT_DIR"
else
    echo "binwalk not found, using dd..."
    # Simple extraction
    dd if="$FIRMWARE_FILE" of="$OUTPUT_DIR/header.bin" bs=1024 count=1
    dd if="$FIRMWARE_FILE" of="$OUTPUT_DIR/kernel.bin" bs=1024 skip=1 count=2048
    dd if="$FIRMWARE_FILE" of="$OUTPUT_DIR/rootfs.bin" bs=1024 skip=2049
fi

echo "Firmware extracted to: $OUTPUT_DIR"

