#include "firmware.h"
#include "boot_config.h"
#include "crypto.h"
#include <stdint.h>
#include <string.h>
#include <stdio.h>

static uint8_t firmware_checksum[SHA256_HASH_SIZE];
static uint32_t checksum_failures = 0;
static int initialized = 0;

// v1.1 - Added failure counter to prevent false positives
// v1.0 - Initial implementation

int anti_tamper_init(void)
{
    /* Calculate initial checksum of critical code sections */
    extern uint8_t __firmware_start[];
    extern uint8_t __firmware_end[];
    size_t firmware_size = __firmware_end - __firmware_start;
    
    if (firmware_size == 0) {
        printf("[ANTI-TAMPER] ERROR: Invalid firmware size\n");
        return -1;
    }
    
    if (crypto_hash_sha256(__firmware_start, firmware_size,
                           firmware_checksum) != CRYPTO_SUCCESS) {
        printf("[ANTI-TAMPER] ERROR: Failed to calculate initial checksum\n");
        return -1;
    }
    
    initialized = 1;
    printf("[ANTI-TAMPER] Initialized (firmware size: %zu bytes)\n", firmware_size);
    return 0;
}

void anti_tamper_check(void)
{
    uint8_t current_checksum[SHA256_HASH_SIZE];
    extern uint8_t __firmware_start[];
    extern uint8_t __firmware_end[];
    size_t firmware_size = __firmware_end - __firmware_start;
    
    if (!initialized) {
        // Not initialized yet - skip check
        return;
    }
    
    /* Calculate current checksum - expensive operation */
    if (crypto_hash_sha256(__firmware_start, firmware_size,
                          current_checksum) != CRYPTO_SUCCESS) {
        checksum_failures++;
        printf("[ANTI-TAMPER] WARNING: Checksum calculation failed (%u failures)\n", 
               checksum_failures);
        if (checksum_failures >= MAX_CHECKSUM_FAILURES) {
            /* Tampering detected - take action */
            printf("[ANTI-TAMPER] CRITICAL: Tampering detected! Halting...\n");
            // TODO: Add secure logging before halt
            while (1) { /* Halt - should trigger watchdog reset */ }
        }
        return;
    }
    
    /* Compare checksums - constant time would be better but this works */
    if (memcmp(firmware_checksum, current_checksum, SHA256_HASH_SIZE) != 0) {
        checksum_failures++;
        printf("[ANTI-TAMPER] WARNING: Checksum mismatch (%u failures)\n", 
               checksum_failures);
        if (checksum_failures >= MAX_CHECKSUM_FAILURES) {
            printf("[ANTI-TAMPER] CRITICAL: Code modification detected! Halting...\n");
            while (1) { /* Halt */ }
        }
    } else {
        // Reset counter on success - might want to keep some history
        if (checksum_failures > 0) {
            printf("[ANTI-TAMPER] Checksum OK (reset failure counter)\n");
        }
        checksum_failures = 0;
    }
}

