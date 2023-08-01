#!/bin/bash
# Complete gateway setup script

set -e

echo "Setting up Secure IoT Gateway..."

# Setup firewall
echo "Configuring firewall..."
./firewall/iptables_rules.sh
nft -f ./firewall/nftables_rules.nft

# Setup WireGuard
echo "Setting up WireGuard..."
./vpn/wireguard/setup_wireguard.sh

# Setup IPSec
echo "Setting up IPSec..."
./vpn/ipsec/setup_ipsec.sh

# Setup OpenVPN
echo "Setting up OpenVPN..."
./vpn/openvpn/setup_openvpn.sh

# Setup Snort
echo "Configuring Snort..."
cp ids_ips/snort/snort.conf /etc/snort/
mkdir -p /etc/snort/rules
cp ids_ips/snort/rules/*.rules /etc/snort/rules/

# Setup Suricata
echo "Configuring Suricata..."
cp ids_ips/suricata/suricata.yaml /etc/suricata/

echo "Gateway setup complete!"

