#!/bin/bash
# Run AFL fuzzer

set -e

INPUT_DIR="afl_input"
OUTPUT_DIR="afl_output"
TARGET="./fuzz_target"

# Create input directory with sample files
mkdir -p $INPUT_DIR
echo -n "FIRM" > $INPUT_DIR/sample1.bin
printf "\x01\x00\x00\x00" >> $INPUT_DIR/sample1.bin  # version
printf "\x10\x00\x00\x00" >> $INPUT_DIR/sample1.bin  # size
printf "\x00\x00\x00\x00" >> $INPUT_DIR/sample1.bin  # checksum

# Build with AFL
afl-gcc -o $TARGET fuzz_target.c

# Run fuzzer
afl-fuzz -i $INPUT_DIR -o $OUTPUT_DIR -- $TARGET @@

