#!/bin/bash
# Create test firmware binary

set -e

OUTPUT="firmware.bin"
SIZE="${1:-10240}"  # Default 10KB

echo "Creating firmware: $OUTPUT (size: $SIZE bytes)"

# Create firmware with header
printf "FIRM" > $OUTPUT  # Magic
printf "\x01\x00\x00\x00" >> $OUTPUT  # Version
printf "\x$(printf '%08x' $SIZE | sed 's/\(..\)/\\x\1/g')" >> $OUTPUT  # Size
printf "\x00\x00\x00\x00" >> $OUTPUT  # Checksum placeholder

# Fill with data
dd if=/dev/zero bs=1 count=$((SIZE - 16)) >> $OUTPUT 2>/dev/null

echo "Firmware created: $OUTPUT"

