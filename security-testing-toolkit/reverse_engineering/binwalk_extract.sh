#!/bin/bash
# Extract firmware using binwalk

FIRMWARE_FILE="${1:-firmware.bin}"
OUTPUT_DIR="extracted_firmware"

if [ ! -f "$FIRMWARE_FILE" ]; then
    echo "Error: Firmware file not found: $FIRMWARE_FILE"
    exit 1
fi

echo "Analyzing firmware: $FIRMWARE_FILE"

# Analyze firmware
binwalk $FIRMWARE_FILE

# Extract files
echo "Extracting files..."
mkdir -p $OUTPUT_DIR
binwalk -e $FIRMWARE_FILE -C $OUTPUT_DIR

# Extract entropy
echo "Analyzing entropy..."
binwalk -E $FIRMWARE_FILE

# Search for strings
echo "Searching for strings..."
strings $FIRMWARE_FILE | head -100

echo "Extraction complete. Files in: $OUTPUT_DIR"

