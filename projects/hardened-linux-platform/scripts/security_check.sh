#!/bin/bash
# Security configuration check

echo "=== Security Configuration Check ==="

# Check IMA
if [ -d /sys/kernel/security/ima ]; then
    echo "IMA: Enabled"
else
    echo "IMA: Disabled"
fi

# Check EVM
if [ -d /sys/kernel/security/evm ]; then
    echo "EVM: Enabled"
else
    echo "EVM: Disabled"
fi

# Check namespaces
echo "=== Namespaces ==="
ip netns list 2>/dev/null && echo "Network namespaces: Active" || echo "Network namespaces: None"

# Check cgroups
echo "=== cgroups ==="
[ -d /sys/fs/cgroup ] && echo "cgroups: Available" || echo "cgroups: Not available"

# Check encryption
echo "=== Encryption ==="
lsblk -f | grep -q crypt && echo "Encrypted devices: Found" || echo "Encrypted devices: None"

echo "Security check complete"

