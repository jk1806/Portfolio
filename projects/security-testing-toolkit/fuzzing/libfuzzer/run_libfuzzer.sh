#!/bin/bash
# Run libFuzzer

set -e

TARGET="fuzz_target"
INPUT_DIR="fuzz_input"
OUTPUT_DIR="fuzz_output"

mkdir -p $INPUT_DIR $OUTPUT_DIR

# Create sample input
echo -n "test" > $INPUT_DIR/sample1

# Build with libFuzzer
clang -fsanitize=fuzzer,address \
      -g -O1 \
      -o $TARGET \
      fuzz_target.c

# Run fuzzer
./$TARGET $INPUT_DIR -artifact_prefix=$OUTPUT_DIR/

