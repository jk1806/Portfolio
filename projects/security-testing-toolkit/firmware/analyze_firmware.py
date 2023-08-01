#!/usr/bin/env python3
"""Analyze extracted firmware"""

import sys
import os
import hashlib
import magic

def analyze_file(filepath):
    """Analyze a firmware file"""
    print(f"\nAnalyzing: {filepath}")
    
    # File type
    mime = magic.Magic(mime=True)
    file_type = mime.from_file(filepath)
    print(f"  Type: {file_type}")
    
    # Size
    size = os.path.getsize(filepath)
    print(f"  Size: {size} bytes")
    
    # Hash
    with open(filepath, 'rb') as f:
        data = f.read()
        md5 = hashlib.md5(data).hexdigest()
        sha256 = hashlib.sha256(data).hexdigest()
        print(f"  MD5: {md5}")
        print(f"  SHA256: {sha256}")
    
    # Search for strings
    strings = []
    current_string = ""
    for byte in data:
        if 32 <= byte < 127:
            current_string += chr(byte)
        else:
            if len(current_string) >= 4:
                strings.append(current_string)
            current_string = ""
    
    if strings:
        print(f"  Strings found: {len(strings)}")
        for s in strings[:10]:  # Show first 10
            print(f"    - {s}")

if __name__ == '__main__':
    if len(sys.argv) < 2:
        print("Usage: analyze_firmware.py <firmware_file>")
        sys.exit(1)
    
    analyze_file(sys.argv[1])

