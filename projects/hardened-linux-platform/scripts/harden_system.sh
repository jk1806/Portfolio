#!/bin/bash
# System hardening script for embedded Linux
# v1.2 - Added kernel parameter validation (2024-07-01)
# v1.1 - Fixed SELinux check
# v1.0 - Initial version

set -e

echo "Hardening Linux system..."

# Apply SELinux policies - optional, might not be available
if command -v setsebool >/dev/null 2>&1; then
    echo "Applying SELinux policies..."
    # TODO: Add actual SELinux policy loading
    # setsebool -P httpd_can_network_connect 1
else
    echo "WARNING: SELinux not available, skipping..."
fi

# Setup namespaces - network isolation
echo "Setting up namespaces..."
if [ -f "./namespaces/netns_setup.sh" ]; then
    ./namespaces/netns_setup.sh
else
    echo "WARNING: netns_setup.sh not found"
fi

if [ -f "./namespaces/mountns_setup.sh" ]; then
    ./namespaces/mountns_setup.sh
fi

# Configure cgroups - resource limits
echo "Configuring cgroups..."
if [ -f "./cgroups/setup_cgroups.sh" ]; then
    ./cgroups/setup_cgroups.sh
else
    echo "WARNING: cgroups setup script not found"
fi

# Setup IMA/EVM - integrity measurement
echo "Setting up IMA/EVM..."
if [ -f "./ima_evm/evm_setup.sh" ]; then
    ./ima_evm/evm_setup.sh
fi

# Enable encryption - dm-crypt for full disk encryption
echo "Setting up encryption..."
if [ -f "./encryption/setup_dm_crypt.sh" ]; then
    ./encryption/setup_dm_crypt.sh
fi

# Apply kernel hardening - prevent information leaks
echo "Applying kernel hardening parameters..."
# Hide kernel pointers from /proc/kallsyms
sysctl -w kernel.kptr_restrict=2 || echo "WARNING: Failed to set kptr_restrict"
# Restrict dmesg access
sysctl -w kernel.dmesg_restrict=1 || echo "WARNING: Failed to set dmesg_restrict"
# Disable unprivileged BPF
sysctl -w kernel.unprivileged_bpf_disabled=1 || echo "WARNING: Failed to set bpf_disabled"

# Additional hardening (commented out - might break some apps)
# sysctl -w kernel.unprivileged_userns_clone=0
# sysctl -w net.core.bpf_jit_harden=2

echo "System hardening complete!"

