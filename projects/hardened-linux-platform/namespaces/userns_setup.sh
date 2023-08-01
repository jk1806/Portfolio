#!/bin/bash
# Setup user namespace

# Create user namespace
unshare --user --map-root-user --fork bash -c "
    echo 'User namespace created'
    echo 'Running as UID: $(id -u)'
    echo 'Running as GID: $(id -g)'
    
    # Keep namespace alive
    exec bash
"

