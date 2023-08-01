#!/bin/bash
# Test SELinux configuration

echo "Testing SELinux..."

# Check if SELinux is enabled
if [ "$(getenforce)" != "Enforcing" ]; then
    echo "SELinux is not in Enforcing mode"
    exit 1
fi

# Check policy
if [ -f /etc/selinux/targeted/policy/policy.* ]; then
    echo "SELinux policy: OK"
else
    echo "SELinux policy: MISSING"
    exit 1
fi

# Test context
echo "Current context: $(id -Z)"

echo "SELinux test PASSED"

