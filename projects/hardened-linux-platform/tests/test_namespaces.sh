#!/bin/bash
# Test namespace isolation

echo "Testing namespaces..."

# Test network namespace
ip netns add test_netns
if ip netns exec test_netns ip link show lo >/dev/null 2>&1; then
    echo "Network namespace: OK"
    ip netns delete test_netns
else
    echo "Network namespace: FAILED"
    exit 1
fi

# Test mount namespace
if unshare --mount --fork true 2>/dev/null; then
    echo "Mount namespace: OK"
else
    echo "Mount namespace: FAILED"
    exit 1
fi

echo "Namespace tests PASSED"

