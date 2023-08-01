#!/bin/bash
# Setup WireGuard VPN

set -e

WG_INTERFACE="wg0"
WG_PORT=51820
WG_CONFIG="/etc/wireguard/${WG_INTERFACE}.conf"

echo "Setting up WireGuard..."

# Generate keys
PRIVATE_KEY=$(wg genkey)
PUBLIC_KEY=$(echo $PRIVATE_KEY | wg pubkey)

# Create config
cat > $WG_CONFIG <<EOF
[Interface]
Address = 10.0.2.1/24
ListenPort = $WG_PORT
PrivateKey = $PRIVATE_KEY

[Peer]
PublicKey = <PEER_PUBLIC_KEY>
AllowedIPs = 10.0.2.2/32
EOF

# Enable interface
wg-quick up $WG_INTERFACE

# Enable IP forwarding
echo 1 > /proc/sys/net/ipv4/ip_forward

# NAT rules
iptables -t nat -A POSTROUTING -o $WG_INTERFACE -j MASQUERADE

echo "WireGuard interface $WG_INTERFACE configured"
echo "Public key: $PUBLIC_KEY"

