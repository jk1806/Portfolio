#!/bin/bash
# Setup Certificate Authority

set -e

CA_DIR="ca"
CA_KEY="$CA_DIR/ca.key"
CA_CERT="$CA_DIR/ca.crt"

echo "Setting up Certificate Authority..."

mkdir -p $CA_DIR

# Generate CA private key
openssl genrsa -out $CA_KEY 4096

# Generate CA certificate
openssl req -new -x509 -days 3650 -key $CA_KEY -out $CA_CERT \
    -subj "/C=US/ST=CA/O=IoT CA/CN=IoT Root CA"

echo "CA setup complete:"
echo "  CA Key: $CA_KEY"
echo "  CA Cert: $CA_CERT"

