#!/bin/bash
# Setup network namespace for isolation

NETNS_NAME="embedded_netns"

# Create network namespace
ip netns add $NETNS_NAME

# Create veth pair
ip link add veth0 type veth peer name veth1

# Move veth1 to namespace
ip link set veth1 netns $NETNS_NAME

# Configure addresses
ip addr add 10.0.1.1/24 dev veth0
ip link set veth0 up

ip netns exec $NETNS_NAME ip addr add 10.0.1.2/24 dev veth1
ip netns exec $NETNS_NAME ip link set veth1 up
ip netns exec $NETNS_NAME ip link set lo up

# Enable forwarding
echo 1 > /proc/sys/net/ipv4/ip_forward

# NAT rules
iptables -t nat -A POSTROUTING -s 10.0.1.0/24 -j MASQUERADE
iptables -A FORWARD -i veth0 -j ACCEPT
iptables -A FORWARD -o veth0 -j ACCEPT

echo "Network namespace $NETNS_NAME created"
echo "To enter: ip netns exec $NETNS_NAME bash"

