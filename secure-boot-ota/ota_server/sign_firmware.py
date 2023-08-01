#!/usr/bin/env python3
"""Sign firmware with RSA private key
v1.0 - Initial implementation
"""

import sys
import hashlib
from cryptography.hazmat.primitives import hashes, serialization
from cryptography.hazmat.primitives.asymmetric import padding
from cryptography.hazmat.backends import default_backend

def sign_firmware(firmware_path, key_path, output_path):
    """Sign firmware file with private key"""
    # Load private key
    with open(key_path, 'rb') as f:
        private_key = serialization.load_pem_private_key(
            f.read(), password=None, backend=default_backend())
    
    # Read firmware
    with open(firmware_path, 'rb') as f:
        firmware_data = f.read()
    
    # Sign firmware
    signature = private_key.sign(
        firmware_data,
        padding.PSS(
            mgf=padding.MGF1(hashes.SHA256()),
            salt_length=padding.PSS.MAX_LENGTH
        ),
        hashes.SHA256()
    )
    
    # Write signature
    with open(output_path, 'wb') as f:
        f.write(signature)
    
    print(f"Firmware signed: {firmware_path}")
    print(f"Signature saved: {output_path}")

if __name__ == '__main__':
    if len(sys.argv) != 4:
        print("Usage: sign_firmware.py <firmware> <private_key> <signature_output>")
        sys.exit(1)
    
    sign_firmware(sys.argv[1], sys.argv[2], sys.argv[3])

