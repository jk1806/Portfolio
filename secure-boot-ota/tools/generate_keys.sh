#!/bin/bash
# Generate keys for secure boot and firmware encryption
# v1.0 - Initial version

set -e

KEYS_DIR="keys"
mkdir -p "$KEYS_DIR"

echo "Generating keys for secure boot..."

# Generate RSA key pair for bootloader
openssl genrsa -out "$KEYS_DIR/boot_private_key.pem" 2048
openssl rsa -in "$KEYS_DIR/boot_private_key.pem" -pubout -out "$KEYS_DIR/boot_public_key.pem"

# Generate ECDSA key pair (alternative)
openssl ecparam -genkey -name prime256v1 -out "$KEYS_DIR/ecdsa_private_key.pem"
openssl ec -in "$KEYS_DIR/ecdsa_private_key.pem" -pubout -out "$KEYS_DIR/ecdsa_public_key.pem"

# Generate AES key for firmware encryption
openssl rand -out "$KEYS_DIR/firmware_key.bin" 32

# Generate HMAC key
openssl rand -out "$KEYS_DIR/hmac_key.bin" 32

echo "Keys generated successfully in $KEYS_DIR/"
echo "  - boot_private_key.pem"
echo "  - boot_public_key.pem"
echo "  - ecdsa_private_key.pem"
echo "  - ecdsa_public_key.pem"
echo "  - firmware_key.bin"
echo "  - hmac_key.bin"

