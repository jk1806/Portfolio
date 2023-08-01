#!/usr/bin/env python3
"""
OTA Update Server
Handles secure firmware updates for embedded devices

v1.3 - Added version validation (2024-06-15)
v1.2 - Fixed file upload size limit
v1.1 - Added signature verification
v1.0 - Initial implementation
"""

import os
import sys
import json
import hashlib
import argparse
from flask import Flask, request, jsonify, send_file
from cryptography.hazmat.primitives import hashes, serialization
from cryptography.hazmat.primitives.asymmetric import rsa, padding
from cryptography.hazmat.backends import default_backend
import base64

app = Flask(__name__)

# Configuration - TODO: Move to config file
FIRMWARE_DIR = "firmware_updates"
KEYS_DIR = "keys"
MAX_FIRMWARE_SIZE = 512 * 1024  # 512 KB - might need to increase

class OTAServer:
    def __init__(self, port=8080):
        self.port = port
        self.firmware_versions = {}
        self.load_firmware_versions()
    
    def load_firmware_versions(self):
        """Load firmware version information"""
        version_file = os.path.join(FIRMWARE_DIR, "versions.json")
        if os.path.exists(version_file):
            with open(version_file, 'r') as f:
                self.firmware_versions = json.load(f)
    
    def save_firmware_versions(self):
        """Save firmware version information"""
        version_file = os.path.join(FIRMWARE_DIR, "versions.json")
        os.makedirs(FIRMWARE_DIR, exist_ok=True)
        with open(version_file, 'w') as f:
            json.dump(self.firmware_versions, f, indent=2)
    
    def get_firmware_info(self, version=None):
        """Get firmware information"""
        if version is None:
            # Return latest version
            if not self.firmware_versions:
                return None
            latest = max(self.firmware_versions.keys(), key=int)
            version = latest
        
        if version not in self.firmware_versions:
            return None
        
        info = self.firmware_versions[version].copy()
        info['version'] = version
        return info
    
    def verify_firmware_signature(self, firmware_path, signature_path):
        """Verify firmware signature - RSA-2048 with PSS padding"""
        try:
            # Load public key - should be in secure storage in production
            pub_key_path = os.path.join(KEYS_DIR, "boot_public_key.pem")
            if not os.path.exists(pub_key_path):
                print(f"ERROR: Public key not found at {pub_key_path}")
                return False
                
            with open(pub_key_path, 'rb') as f:
                public_key = serialization.load_pem_public_key(
                    f.read(), backend=default_backend())
            
            # Read firmware
            with open(firmware_path, 'rb') as f:
                firmware_data = f.read()
            
            # Read signature
            if not os.path.exists(signature_path):
                print(f"ERROR: Signature file not found: {signature_path}")
                return False
                
            with open(signature_path, 'rb') as f:
                signature = f.read()
            
            # Verify signature - PSS is more secure than PKCS#1 v1.5
            public_key.verify(
                signature,
                firmware_data,
                padding.PSS(
                    mgf=padding.MGF1(hashes.SHA256()),
                    salt_length=padding.PSS.MAX_LENGTH
                ),
                hashes.SHA256()
            )
            return True
        except Exception as e:
            print(f"Signature verification failed: {e}")
            # TODO: Log to secure audit log
            return False

@app.route('/api/firmware/info', methods=['GET'])
def get_firmware_info():
    """Get firmware information"""
    version = request.args.get('version')
    server = app.config['ota_server']
    info = server.get_firmware_info(version)
    
    if info is None:
        return jsonify({'error': 'Firmware not found'}), 404
    
    return jsonify(info)

@app.route('/api/firmware/download', methods=['GET'])
def download_firmware():
    """Download firmware"""
    version = request.args.get('version')
    server = app.config['ota_server']
    info = server.get_firmware_info(version)
    
    if info is None:
        return jsonify({'error': 'Firmware not found'}), 404
    
    firmware_path = os.path.join(FIRMWARE_DIR, info['filename'])
    if not os.path.exists(firmware_path):
        return jsonify({'error': 'Firmware file not found'}), 404
    
    return send_file(firmware_path, as_attachment=True)

@app.route('/api/firmware/update', methods=['POST'])
def update_firmware():
    """Upload new firmware - TODO: Add authentication"""
    if 'firmware' not in request.files:
        return jsonify({'error': 'No firmware file'}), 400
    
    firmware_file = request.files['firmware']
    version = request.form.get('version')
    
    if not version:
        return jsonify({'error': 'Version required'}), 400
    
    # Basic version validation - should be more strict
    try:
        version_int = int(version)
        if version_int <= 0:
            return jsonify({'error': 'Invalid version number'}), 400
    except ValueError:
        return jsonify({'error': 'Version must be numeric'}), 400
    
    # Save firmware
    os.makedirs(FIRMWARE_DIR, exist_ok=True)
    filename = f"firmware_v{version}.bin"
    firmware_path = os.path.join(FIRMWARE_DIR, filename)
    
    # Check file size before saving
    firmware_file.seek(0, os.SEEK_END)
    file_size = firmware_file.tell()
    firmware_file.seek(0)
    
    if file_size > MAX_FIRMWARE_SIZE:
        return jsonify({'error': f'Firmware too large ({file_size} > {MAX_FIRMWARE_SIZE})'}), 400
    
    firmware_file.save(firmware_path)
    
    # Calculate hash - SHA256 for integrity
    with open(firmware_path, 'rb') as f:
        firmware_data = f.read()
        sha256_hash = hashlib.sha256(firmware_data).hexdigest()
    
    # Update version info
    server = app.config['ota_server']
    server.firmware_versions[version] = {
        'filename': filename,
        'size': len(firmware_data),
        'sha256': sha256_hash,
        'timestamp': os.path.getmtime(firmware_path)
    }
    server.save_firmware_versions()
    
    print(f"Firmware uploaded: version {version}, size {len(firmware_data)} bytes")
    
    return jsonify({
        'success': True,
        'version': version,
        'sha256': sha256_hash
    })

def main():
    parser = argparse.ArgumentParser(description='OTA Update Server')
    parser.add_argument('--port', type=int, default=8080, help='Server port')
    parser.add_argument('--host', default='0.0.0.0', help='Server host')
    args = parser.parse_args()
    
    # Initialize server
    server = OTAServer(port=args.port)
    app.config['ota_server'] = server
    
    # Create directories if they don't exist
    os.makedirs(FIRMWARE_DIR, exist_ok=True)
    os.makedirs(KEYS_DIR, exist_ok=True)
    
    print(f"Starting OTA Server on {args.host}:{args.port}")
    print(f"Firmware directory: {FIRMWARE_DIR}")
    print(f"Keys directory: {KEYS_DIR}")
    # FIXME: debug=True should be False in production!
    app.run(host=args.host, port=args.port, debug=True)

if __name__ == '__main__':
    main()

