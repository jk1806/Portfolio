#!/bin/bash
# Setup AppArmor

set -e

echo "Setting up AppArmor..."

# Install AppArmor if needed
if ! command -v aa-status >/dev/null 2>&1; then
    echo "Installing AppArmor..."
    apt-get update
    apt-get install -y apparmor apparmor-utils
fi

# Load profiles
aa-enforce /etc/apparmor.d/embedded_app

# Check status
aa-status

echo "AppArmor setup complete"

