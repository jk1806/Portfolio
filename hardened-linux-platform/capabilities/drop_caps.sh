#!/bin/bash
# Drop Linux capabilities for least privilege

PROCESS="$1"

if [ -z "$PROCESS" ]; then
    echo "Usage: $0 <process_name>"
    exit 1
fi

# Drop all capabilities except required ones
# Keep only: CAP_NET_BIND_SERVICE, CAP_SYS_TIME (example)
capsh --drop=all --add=cap_net_bind_service,cap_sys_time -- "$PROCESS"

echo "Capabilities dropped for: $PROCESS"

