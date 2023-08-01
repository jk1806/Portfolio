#!/bin/bash
# Setup cgroups for resource limits

CGROUP_NAME="embedded_app"
CGROUP_PATH="/sys/fs/cgroup/$CGROUP_NAME"

# Create cgroup
mkdir -p $CGROUP_PATH

# Set CPU limit (50% of one core)
echo "50000" > $CGROUP_PATH/cpu.cfs_quota_us
echo "100000" > $CGROUP_PATH/cpu.cfs_period_us

# Set memory limit (256 MB)
echo "256M" > $CGROUP_PATH/memory.max

# Set I/O limits
echo "8:0 1048576" > $CGROUP_PATH/io.max
# 8:0 is major:minor for block device, 1048576 is 1MB/s

echo "Cgroup $CGROUP_NAME configured"
echo "To add process: echo \$PID > $CGROUP_PATH/cgroup.procs"

