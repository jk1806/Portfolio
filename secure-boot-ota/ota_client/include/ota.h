#ifndef OTA_H
#define OTA_H

#include <stdint.h>
#include <stddef.h>

int ota_download_firmware(const char *server_url, const char *version,
                         uint8_t **firmware_data, size_t *firmware_size);
int ota_verify_firmware(const uint8_t *firmware, size_t size,
                       const uint8_t *signature, size_t sig_len);
int ota_install_firmware(const uint8_t *firmware, size_t size);

#endif /* OTA_H */

