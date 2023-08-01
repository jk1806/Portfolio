#!/bin/bash
# iptables firewall rules for IoT gateway
# v1.3 - Added rate limiting (2024-08-10)
# v1.2 - Fixed SSH port
# v1.1 - Added logging
# v1.0 - Initial ruleset

# Flush existing rules - careful! This drops all connections
iptables -F
iptables -X
iptables -t nat -F
iptables -t nat -X

# Default policies - deny by default
iptables -P INPUT DROP
iptables -P FORWARD DROP
iptables -P OUTPUT ACCEPT

# Allow loopback - needed for local services
iptables -A INPUT -i lo -j ACCEPT
iptables -A OUTPUT -o lo -j ACCEPT

# Allow established connections - important for return traffic
iptables -A INPUT -m state --state ESTABLISHED,RELATED -j ACCEPT

# Allow SSH - TODO: Should restrict to specific IPs
iptables -A INPUT -p tcp --dport 22 -j ACCEPT
# Rate limit SSH to prevent brute force
iptables -A INPUT -p tcp --dport 22 -m recent --name ssh --set
iptables -A INPUT -p tcp --dport 22 -m recent --name ssh --update --seconds 60 --hitcount 5 -j DROP

# Allow MQTT-TLS - encrypted MQTT
iptables -A INPUT -p tcp --dport 8883 -j ACCEPT

# Allow CoAP-DTLS - encrypted CoAP
iptables -A INPUT -p udp --dport 5684 -j ACCEPT

# Allow HTTPS - for web interface
iptables -A INPUT -p tcp --dport 443 -j ACCEPT

# Log dropped packets - might be too verbose in production
iptables -A INPUT -j LOG --log-prefix "IPTABLES-DROPPED: " --log-level 4

# TODO: Add IPv6 rules (ip6tables)
# TODO: Add fail2ban integration

echo "iptables rules applied"

