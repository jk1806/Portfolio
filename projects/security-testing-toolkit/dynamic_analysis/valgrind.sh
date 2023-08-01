#!/bin/bash
# Run Valgrind memory analysis

TARGET="${1:-./test_program}"

if [ ! -f "$TARGET" ]; then
    echo "Error: Target not found: $TARGET"
    exit 1
fi

echo "Running Valgrind memory check..."

valgrind --leak-check=full \
         --show-leak-kinds=all \
         --track-origins=yes \
         --verbose \
         --log-file=valgrind.log \
         $TARGET

echo "Valgrind report saved to: valgrind.log"

