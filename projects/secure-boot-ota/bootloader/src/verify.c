#include "boot.h"
#include "crypto.h"
#include "boot_config.h"
#include <string.h>
#include <stdio.h>

extern uint8_t boot_public_key[];

int verify_firmware_signature(const uint8_t *firmware, size_t size,
                              const uint8_t *signature, size_t sig_len)
{
    int ret;
    
    if (!firmware || !signature || size == 0) {
        return -1;
    }
    
    /* Verify RSA signature */
    ret = crypto_verify_rsa(firmware, size, signature, sig_len,
                           boot_public_key, BOOT_PUBLIC_KEY_SIZE);
    
    if (ret != CRYPTO_SUCCESS) {
        return -1;
    }
    
    return 0;
}

/* verify_firmware_integrity() is implemented in integrity.c */

