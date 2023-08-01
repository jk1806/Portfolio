#!/bin/bash
# Static analysis using cppcheck
# v1.1 - Added XML output (2024-07-15)
# v1.0 - Initial version

set -e

SOURCE_DIR="${1:-.}"
OUTPUT_DIR="reports/cppcheck"
mkdir -p "$OUTPUT_DIR"

echo "Running cppcheck on $SOURCE_DIR..."

# XML output for CI/CD integration
cppcheck \
    --enable=all \
    --suppress=missingIncludeSystem \
    --xml \
    --xml-version=2 \
    --output-file="$OUTPUT_DIR/report.xml" \
    "$SOURCE_DIR" || true  # Don't fail on warnings

# Human-readable output
cppcheck \
    --enable=all \
    --suppress=missingIncludeSystem \
    --output-file="$OUTPUT_DIR/report.txt" \
    "$SOURCE_DIR" || true

# TODO: Add --platform option for embedded targets
# TODO: Add custom rule files

echo "cppcheck analysis complete. Report: $OUTPUT_DIR/report.txt"

