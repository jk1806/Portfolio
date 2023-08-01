#include "boot.h"
#include "crypto.h"
#include "boot_config.h"
#include <string.h>

// v1.1 - Added GCM support (2024-04-10)
// v1.0 - Initial CTR mode only

extern uint8_t firmware_key[];

int decrypt_firmware(uint8_t *firmware, size_t size)
{
    uint8_t iv[16];
    int ret;
    
    if (!firmware || size == 0) {
        return -1;
    }
    
    /* Extract IV from firmware header - first 16 bytes */
    memcpy(iv, firmware, 16);
    
#if FIRMWARE_ENCRYPTION == FIRMWARE_ENCRYPTION_AES_256_GCM
    // GCM mode - authenticated encryption
    uint8_t tag[16];
    uint8_t *ciphertext = firmware + 16;
    size_t ct_len = size - 16 - 16; /* Subtract IV and tag */
    
    /* Extract tag */
    memcpy(tag, firmware + size - 16, 16);
    
    /* Decrypt */
    ret = crypto_decrypt_aes_gcm(ciphertext, ct_len,
                                 firmware_key, iv,
                                 NULL, 0, tag,
                                 ciphertext);
#else
    /* AES-CTR mode */
    ret = crypto_decrypt_aes_ctr(firmware + 16, size - 16,
                                firmware_key, iv,
                                firmware + 16);
#endif
    
    if (ret != CRYPTO_SUCCESS) {
        return -1;
    }
    
    return 0;
}

