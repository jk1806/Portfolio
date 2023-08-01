#include "boot.h"
#include "crypto.h"
#include <string.h>

// v1.0 - Initial implementation
// HMAC-based integrity verification

extern uint8_t hmac_key[];

int verify_firmware_integrity(const uint8_t *firmware, size_t size)
{
    uint8_t calculated_hmac[HMAC_SIZE];
    uint8_t stored_hmac[HMAC_SIZE];
    int ret;
    
    if (!firmware || size < HMAC_SIZE) {
        return -1;
    }
    
    /* Extract stored HMAC from end of firmware */
    memcpy(stored_hmac, firmware + size - HMAC_SIZE, HMAC_SIZE);
    
    /* Calculate HMAC over firmware (excluding HMAC itself) */
    ret = crypto_hmac_sha256(firmware, size - HMAC_SIZE,
                             hmac_key, 32,
                             calculated_hmac);
    if (ret != CRYPTO_SUCCESS) {
        return -1;
    }
    
    /* Compare HMACs */
    if (memcmp(stored_hmac, calculated_hmac, HMAC_SIZE) != 0) {
        return -1; /* Integrity check failed */
    }
    
    return 0;
}

