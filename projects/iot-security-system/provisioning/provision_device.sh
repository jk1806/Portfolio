#!/bin/bash
# Device provisioning script

set -e

DEVICE_ID="${1:-device-001}"
CA_CERT="ca.crt"
CA_KEY="ca.key"

echo "Provisioning device: $DEVICE_ID"

# Generate device certificate
python3 identity/cert_generator.py \
    --device-id $DEVICE_ID \
    --ca-cert $CA_CERT \
    --ca-key $CA_KEY \
    --output "device_${DEVICE_ID}.crt"

# Generate device key
openssl genrsa -out "device_${DEVICE_ID}.key" 2048

# Create device configuration
cat > "device_${DEVICE_ID}.conf" <<EOF
DEVICE_ID=$DEVICE_ID
CERT_FILE=device_${DEVICE_ID}.crt
KEY_FILE=device_${DEVICE_ID}.key
CA_CERT=$CA_CERT
SERVER_URL=https://iot-server.example.com
EOF

echo "Device $DEVICE_ID provisioned successfully"
echo "Configuration saved to: device_${DEVICE_ID}.conf"

