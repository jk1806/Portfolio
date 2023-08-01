#ifndef BOOT_H
#define BOOT_H

#include <stdint.h>
#include <stddef.h>

/* Bootloader functions */
int bootloader_main(void);
int verify_firmware_signature(const uint8_t *firmware, size_t size);
int decrypt_firmware(uint8_t *firmware, size_t size);
int verify_firmware_integrity(const uint8_t *firmware, size_t size);
int check_version_counter(uint32_t *version);
int update_version_counter(uint32_t version);
void jump_to_firmware(void *address);

/* Constants */
#define BOOT_PUBLIC_KEY_ADDRESS 0x0800E000

#endif /* BOOT_H */

