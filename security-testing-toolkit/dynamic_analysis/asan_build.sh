#!/bin/bash
# Build with AddressSanitizer

set -e

SOURCE="${1:-test.c}"
OUTPUT="${2:-test_asan}"

echo "Building with AddressSanitizer: $SOURCE"

gcc -fsanitize=address \
    -fno-omit-frame-pointer \
    -g \
    -O1 \
    -o "$OUTPUT" \
    "$SOURCE"

echo "Built: $OUTPUT"
echo "Run with: ./$OUTPUT"

