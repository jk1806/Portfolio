#!/bin/bash
# Setup OpenVPN server

set -e

echo "Setting up OpenVPN..."

# Install OpenVPN if needed
if ! command -v openvpn >/dev/null 2>&1; then
    echo "Installing OpenVPN..."
    apt-get update
    apt-get install -y openvpn easy-rsa
fi

# Generate certificates if not exist
if [ ! -f server.crt ]; then
    echo "Generating certificates..."
    make-cadir ~/openvpn-ca
    cd ~/openvpn-ca
    ./easyrsa init-pki
    ./easyrsa build-ca
    ./easyrsa gen-req server nopass
    ./easyrsa sign-req server server
    ./easyrsa gen-dh
    cp pki/ca.crt pki/issued/server.crt pki/private/server.key pki/dh.pem ../
    cd ..
fi

# Copy configuration
cp server.conf /etc/openvpn/server/

# Start OpenVPN
systemctl enable openvpn@server
systemctl start openvpn@server

echo "OpenVPN setup complete"

