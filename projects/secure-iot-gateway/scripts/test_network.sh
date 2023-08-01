#!/bin/bash
# Network connectivity test

echo "Testing network connectivity..."

# Test firewall
echo "=== Firewall Test ==="
iptables -L -n | grep -q "DROP" && echo "Firewall: Active" || echo "Firewall: Inactive"

# Test WireGuard
echo "=== WireGuard Test ==="
if ip link show wg0 >/dev/null 2>&1; then
    echo "WireGuard: Active"
    wg show
else
    echo "WireGuard: Inactive"
fi

# Test IPSec
echo "=== IPSec Test ==="
if ipsec status >/dev/null 2>&1; then
    echo "IPSec: Active"
    ipsec status
else
    echo "IPSec: Inactive"
fi

# Test TLS server
echo "=== TLS Server Test ==="
timeout 2 bash -c 'echo > /dev/tcp/localhost/4433' 2>/dev/null && echo "TLS: Listening" || echo "TLS: Not listening"

echo "Network tests complete"

