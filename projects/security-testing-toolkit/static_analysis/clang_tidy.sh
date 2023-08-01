#!/bin/bash
# Static analysis using clang-tidy

set -e

SOURCE_DIR="${1:-.}"
OUTPUT_DIR="reports/clang-tidy"
mkdir -p "$OUTPUT_DIR"

echo "Running clang-tidy on $SOURCE_DIR..."

# Create compile_commands.json if not exists
if [ ! -f compile_commands.json ]; then
    echo "Generating compile_commands.json..."
    bear -- make 2>/dev/null || echo "Warning: bear not available"
fi

# Run clang-tidy
clang-tidy \
    -p . \
    "$SOURCE_DIR"/*.c \
    "$SOURCE_DIR"/*.cpp \
    > "$OUTPUT_DIR/report.txt" 2>&1 || true

echo "clang-tidy analysis complete. Report: $OUTPUT_DIR/report.txt"

