#ifndef BOOT_CONFIG_H
#define BOOT_CONFIG_H

/* Signature Algorithm */
#define BOOT_SIGNATURE_ALGORITHM_RSA_2048  1
#define BOOT_SIGNATURE_ALGORITHM_ECDSA_P256 2
#define BOOT_SIGNATURE_ALGORITHM BOOT_SIGNATURE_ALGORITHM_RSA_2048

/* Encryption Algorithm */
#define FIRMWARE_ENCRYPTION_AES_256_CTR  1
#define FIRMWARE_ENCRYPTION_AES_256_GCM  2
#define FIRMWARE_ENCRYPTION FIRMWARE_ENCRYPTION_AES_256_GCM

/* Memory Layout */
#define BOOTLOADER_START_ADDRESS  0x08000000
#define FIRMWARE_START_ADDRESS    0x08010000
#define VERSION_COUNTER_ADDRESS   0x0800F000
#define MAX_FIRMWARE_SIZE         (512 * 1024)  /* 512 KB */

/* Key Storage */
#define BOOT_PUBLIC_KEY_SIZE      256  /* RSA-2048 public key */
#define FIRMWARE_KEY_SIZE         32   /* AES-256 key */

/* Version Counter */
#define VERSION_COUNTER_SIZE      4
#define MIN_VERSION               1

/* Anti-Tamper */
#define ENABLE_ANTI_TAMPER        1
#define CHECKSUM_INTERVAL_MS      1000
#define MAX_CHECKSUM_FAILURES     3

/* OTA Configuration */
#define OTA_SERVER_URL_MAX        256
#define OTA_DOWNLOAD_TIMEOUT_MS    30000
#define OTA_MAX_RETRIES           3

#endif /* BOOT_CONFIG_H */

