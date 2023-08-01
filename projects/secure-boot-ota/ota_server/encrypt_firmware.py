#!/usr/bin/env python3
"""Encrypt firmware with AES-GCM
v1.0 - Initial implementation
"""

import sys
import os
from cryptography.hazmat.primitives.ciphers.aead import AESGCM
from cryptography.hazmat.backends import default_backend

def encrypt_firmware(firmware_path, key_path, output_path):
    """Encrypt firmware file with AES-GCM"""
    # Load key
    with open(key_path, 'rb') as f:
        key = f.read()
    
    if len(key) != 32:
        raise ValueError("Key must be 32 bytes for AES-256")
    
    # Read firmware
    with open(firmware_path, 'rb') as f:
        firmware_data = f.read()
    
    # Generate nonce (IV)
    aesgcm = AESGCM(key)
    nonce = os.urandom(12)  # 12 bytes for GCM
    
    # Encrypt
    ciphertext = aesgcm.encrypt(nonce, firmware_data, None)
    
    # Write: nonce (12 bytes) + ciphertext
    with open(output_path, 'wb') as f:
        f.write(nonce)
        f.write(ciphertext)
    
    print(f"Firmware encrypted: {firmware_path}")
    print(f"Encrypted firmware saved: {output_path}")

if __name__ == '__main__':
    if len(sys.argv) != 4:
        print("Usage: encrypt_firmware.py <firmware> <key> <encrypted_output>")
        sys.exit(1)
    
    encrypt_firmware(sys.argv[1], sys.argv[2], sys.argv[3])

