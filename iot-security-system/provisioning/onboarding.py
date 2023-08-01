#!/usr/bin/env python3
"""Device onboarding automation"""

import sys
import json
import requests
from cryptography import x509
from cryptography.hazmat.primitives import serialization

def onboard_device(device_id, ca_cert_path, server_url):
    """Onboard device to IoT platform"""
    
    # Load device certificate
    with open(f"device_{device_id}.crt", "rb") as f:
        device_cert = x509.load_pem_x509_certificate(f.read())
    
    # Create onboarding request
    request_data = {
        "device_id": device_id,
        "certificate": device_cert.public_bytes(serialization.Encoding.PEM).decode(),
        "capabilities": ["telemetry", "commands", "ota"]
    }
    
    # Send to server
    response = requests.post(
        f"{server_url}/api/devices/onboard",
        json=request_data,
        verify=ca_cert_path
    )
    
    if response.status_code == 200:
        result = response.json()
        print(f"Device {device_id} onboarded successfully")
        print(f"Device token: {result.get('token', 'N/A')}")
        return 0
    else:
        print(f"Onboarding failed: {response.status_code}")
        return -1

if __name__ == '__main__':
    if len(sys.argv) != 4:
        print("Usage: onboarding.py <device_id> <ca_cert> <server_url>")
        sys.exit(1)
    
    sys.exit(onboard_device(sys.argv[1], sys.argv[2], sys.argv[3]))

