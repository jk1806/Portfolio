#!/bin/bash
# Sign firmware with RSA private key
# v1.0 - Initial version

set -e

FIRMWARE_FILE="$1"
KEYS_DIR="keys"
PRIVATE_KEY="$KEYS_DIR/boot_private_key.pem"
SIGNATURE_FILE="${FIRMWARE_FILE}.sig"

if [ -z "$FIRMWARE_FILE" ]; then
    echo "Usage: $0 <firmware_file>"
    exit 1
fi

if [ ! -f "$FIRMWARE_FILE" ]; then
    echo "Error: Firmware file not found: $FIRMWARE_FILE"
    exit 1
fi

if [ ! -f "$PRIVATE_KEY" ]; then
    echo "Error: Private key not found: $PRIVATE_KEY"
    echo "Run generate_keys.sh first"
    exit 1
fi

echo "Signing firmware: $FIRMWARE_FILE"

# Sign firmware
openssl dgst -sha256 -sign "$PRIVATE_KEY" -out "$SIGNATURE_FILE" "$FIRMWARE_FILE"

echo "Signature saved to: $SIGNATURE_FILE"

