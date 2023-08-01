#!/bin/bash
# Demo script for secure boot and OTA system

set -e

echo "=========================================="
echo "Secure Boot + OTA Firmware System Demo"
echo "=========================================="
echo ""

# Check prerequisites
command -v openssl >/dev/null 2>&1 || { echo "openssl required"; exit 1; }
command -v python3 >/dev/null 2>&1 || { echo "python3 required"; exit 1; }

# Generate keys if not exists
if [ ! -d "keys" ] || [ ! -f "keys/boot_public_key.pem" ]; then
    echo "Generating keys..."
    cd tools
    ./generate_keys.sh
    cd ..
fi

# Build project
echo "Building project..."
mkdir -p build
cd build
cmake ..
make
cd ..

# Create test firmware
echo "Creating test firmware..."
echo "Test firmware v1.0" > test_firmware.bin
dd if=/dev/zero bs=1024 count=10 >> test_firmware.bin 2>/dev/null

# Sign firmware
echo "Signing firmware..."
cd tools
./sign_firmware.sh ../test_firmware.bin
cd ..

# Start OTA server
echo "Starting OTA server..."
cd ota_server
python3 server.py --port 8080 &
SERVER_PID=$!
cd ..
sleep 2

# Run bootloader simulation
echo "Running bootloader simulation..."
cd build
./bootloader_sim ../test_firmware.bin.signed
cd ..

# Cleanup
echo "Stopping OTA server..."
kill $SERVER_PID 2>/dev/null || true

echo ""
echo "Demo completed!"

