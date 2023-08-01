#!/bin/bash
# Setup GRE (Generic Routing Encapsulation) tunnel

set -e

GRE_INTERFACE="gre0"
LOCAL_IP="${1:-192.168.1.1}"
REMOTE_IP="${2:-10.0.0.1}"

echo "Setting up GRE tunnel..."

# Load GRE module
modprobe ip_gre

# Create GRE tunnel
ip tunnel add $GRE_INTERFACE mode gre remote $REMOTE_IP local $LOCAL_IP ttl 255

# Configure IP address
ip addr add 10.1.1.1/30 dev $GRE_INTERFACE

# Bring interface up
ip link set $GRE_INTERFACE up

# Add route
ip route add 10.0.0.0/8 via 10.1.1.2 dev $GRE_INTERFACE

echo "GRE tunnel $GRE_INTERFACE created"
echo "  Local: $LOCAL_IP"
echo "  Remote: $REMOTE_IP"
echo "  Tunnel IP: 10.1.1.1/30"

