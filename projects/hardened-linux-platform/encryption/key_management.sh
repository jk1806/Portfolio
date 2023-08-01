#!/bin/bash
# Key management for encryption

KEY_DIR="/etc/keys"
KEY_FILE="$KEY_DIR/dmcrypt.key"

case "$1" in
    generate)
        echo "Generating encryption key..."
        mkdir -p "$KEY_DIR"
        dd if=/dev/urandom of="$KEY_FILE" bs=512 count=8
        chmod 600 "$KEY_FILE"
        echo "Key generated: $KEY_FILE"
        ;;
    backup)
        if [ ! -f "$KEY_FILE" ]; then
            echo "Error: Key not found"
            exit 1
        fi
        BACKUP_FILE="${KEY_FILE}.backup.$(date +%Y%m%d)"
        cp "$KEY_FILE" "$BACKUP_FILE"
        chmod 600 "$BACKUP_FILE"
        echo "Key backed up to: $BACKUP_FILE"
        ;;
    *)
        echo "Usage: $0 {generate|backup}"
        exit 1
        ;;
esac

