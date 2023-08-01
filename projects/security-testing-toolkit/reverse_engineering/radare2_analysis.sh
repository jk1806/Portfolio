#!/bin/bash
# Radare2 firmware analysis

FIRMWARE="${1:-firmware.bin}"

if [ ! -f "$FIRMWARE" ]; then
    echo "Error: Firmware not found: $FIRMWARE"
    exit 1
fi

echo "Analyzing firmware with Radare2: $FIRMWARE"

# Open firmware
r2 -A "$FIRMWARE" <<EOF
# Analyze all
aaa

# List functions
afl

# Disassemble main
pdf @main

# Search for strings
iz

# Find crypto functions
/aes
/encrypt
/decrypt

# Export analysis
pdf > radare2_analysis.txt

# Quit
q
EOF

echo "Analysis complete. Results: radare2_analysis.txt"

