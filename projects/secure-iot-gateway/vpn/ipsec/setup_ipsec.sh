#!/bin/bash
# Setup IPSec VPN

set -e

echo "Setting up IPSec..."

# Install strongSwan if needed
if ! command -v ipsec >/dev/null 2>&1; then
    echo "Installing strongSwan..."
    apt-get update
    apt-get install -y strongswan strongswan-pki
fi

# Copy configuration
cp ipsec.conf /etc/ipsec.conf
cp ipsec.secrets /etc/ipsec.secrets 2>/dev/null || echo "Create ipsec.secrets manually"

# Start IPSec
ipsec start
ipsec reload

echo "IPSec setup complete"

