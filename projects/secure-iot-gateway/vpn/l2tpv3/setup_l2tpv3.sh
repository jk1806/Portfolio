#!/bin/bash
# Setup L2TPv3 tunnel

set -e

L2TP_INTERFACE="l2tpeth0"
LOCAL_IP="${1:-192.168.1.1}"
REMOTE_IP="${2:-10.0.0.1}"
SESSION_ID="${3:-100}"
COOKIE="${4:-0x12345678}"

echo "Setting up L2TPv3 tunnel..."

# Load L2TP module
modprobe l2tp_eth

# Create L2TPv3 tunnel
ip l2tp add tunnel \
    tunnel_id 1 \
    peer_tunnel_id 1 \
    encap udp \
    local $LOCAL_IP \
    remote $REMOTE_IP \
    udp_sport 5000 \
    udp_dport 5000

# Create L2TPv3 session
ip l2tp add session \
    tunnel_id 1 \
    session_id $SESSION_ID \
    peer_session_id $SESSION_ID \
    cookie $COOKIE \
    peer_cookie $COOKIE

# Create interface
ip l2tp add session name $L2TP_INTERFACE tunnel_id 1 session_id $SESSION_ID

# Configure IP address
ip addr add 10.2.1.1/30 dev $L2TP_INTERFACE

# Bring interface up
ip link set $L2TP_INTERFACE up

echo "L2TPv3 tunnel created: $L2TP_INTERFACE"
echo "  Local: $LOCAL_IP"
echo "  Remote: $REMOTE_IP"
echo "  Session ID: $SESSION_ID"

