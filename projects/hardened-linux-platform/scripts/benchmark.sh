#!/bin/bash
# Security benchmark script

echo "Running security benchmarks..."

# CIS Benchmark checks
echo "=== CIS Benchmark Checks ==="

# Check password policy
if [ -f /etc/pam.d/common-password ]; then
    echo "Password policy: OK"
else
    echo "Password policy: MISSING"
fi

# Check firewall
if command -v iptables >/dev/null 2>&1; then
    iptables -L | grep -q "INPUT.*DROP" && echo "Firewall: OK" || echo "Firewall: WARNING"
fi

# Check SELinux
if [ -f /etc/selinux/config ]; then
    grep -q "SELINUX=enforcing" /etc/selinux/config && echo "SELinux: OK" || echo "SELinux: WARNING"
fi

# Check kernel parameters
echo "=== Kernel Hardening ==="
[ "$(sysctl -n kernel.randomize_va_space)" = "2" ] && echo "ASLR: OK" || echo "ASLR: WARNING"
[ "$(sysctl -n kernel.kptr_restrict)" = "2" ] && echo "Kptr restrict: OK" || echo "Kptr restrict: WARNING"

echo "Benchmark complete"

