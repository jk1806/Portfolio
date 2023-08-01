#include "boot.h"
#include "boot_config.h"
#include "crypto.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>

/* Simulated flash memory - TODO: Replace with actual flash driver */
static uint8_t flash_memory[1024 * 1024];  /* 1MB flash simulation */

// Bootloader v1.3 - Added better error messages (2024-04-20)
// v1.2 - Fixed version counter rollback bug
// v1.1 - Added HMAC verification
// v1.0 - Initial secure boot implementation

/* Bootloader entry point */
int bootloader_main(void)
{
    int ret;
    uint8_t *firmware = (uint8_t *)FIRMWARE_START_ADDRESS;
    uint32_t firmware_size;
    uint8_t signature[256];
    uint32_t version;
    
    printf("[BOOT] Secure Bootloader v1.3\n");
    printf("[BOOT] Initializing...\n");
    
    /* Initialize crypto - must succeed or boot fails */
    ret = crypto_init();
    if (ret != 0) {
        printf("[BOOT] ERROR: Crypto init failed (code: %d)\n", ret);
        // TODO: Add watchdog reset here
        return -1;
    }
    
    /* Read firmware from flash - offset 0x1000 for header */
    printf("[BOOT] Reading firmware from flash...\n");
    memcpy(&firmware_size, firmware, sizeof(uint32_t));
    firmware += sizeof(uint32_t);
    
    // Sanity check - prevent buffer overflow
    if (firmware_size > MAX_FIRMWARE_SIZE) {
        printf("[BOOT] ERROR: Firmware too large (%u > %u)\n", 
               firmware_size, MAX_FIRMWARE_SIZE);
        return -1;
    }
    if (firmware_size == 0) {
        printf("[BOOT] ERROR: Invalid firmware size\n");
        return -1;
    }
    
    /* Read signature - stored after firmware */
    memcpy(signature, firmware + firmware_size, 256);
    
    /* Verify signature - RSA-2048 or ECDSA-P256 */
    printf("[BOOT] Verifying firmware signature...\n");
    ret = crypto_verify_rsa(firmware, firmware_size,
                           signature, 256,
                           (uint8_t *)BOOT_PUBLIC_KEY_ADDRESS, BOOT_PUBLIC_KEY_SIZE);
    if (ret != CRYPTO_SUCCESS) {
        printf("[BOOT] ERROR: Signature verification failed (code: %d)\n", ret);
        // FIXME: Log this to secure storage for forensics
        return -1;
    }
    printf("[BOOT] Signature verified OK\n");
    
    /* Check version counter - prevent rollback attacks */
    printf("[BOOT] Checking version counter...\n");
    ret = check_version_counter(&version);
    if (ret != 0) {
        printf("[BOOT] ERROR: Version check failed\n");
        return -1;
    }
    printf("[BOOT] Current version: %u\n", version);
    
    /* Decrypt firmware - AES-256-GCM */
    printf("[BOOT] Decrypting firmware...\n");
    ret = decrypt_firmware(firmware, firmware_size);
    if (ret != 0) {
        printf("[BOOT] ERROR: Decryption failed\n");
        return -1;
    }
    printf("[BOOT] Firmware decrypted OK\n");
    
    /* Verify HMAC - integrity check */
    printf("[BOOT] Verifying integrity...\n");
    ret = verify_firmware_integrity(firmware, firmware_size);
    if (ret != 0) {
        printf("[BOOT] ERROR: Integrity check failed\n");
        return -1;
    }
    printf("[BOOT] Integrity verified OK\n");
    
    /* Update version counter - increment before boot */
    version++;
    update_version_counter(version);
    
    printf("[BOOT] Booting firmware (version %u)...\n", version);
    
    /* Jump to firmware - point of no return */
    jump_to_firmware((void *)firmware);
    
    /* Should not reach here - if we do, something went wrong */
    printf("[BOOT] ERROR: Jump failed!\n");
    return -1;
}

