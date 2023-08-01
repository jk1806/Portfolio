#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <openssl/hmac.h>
#include <openssl/sha.h>

#define HMAC_KEY_SIZE 32
#define MAX_COMMAND_SIZE 1024

typedef struct {
    uint8_t command_id;
    uint32_t timestamp;
    uint8_t data[MAX_COMMAND_SIZE];
    uint8_t hmac[32];
} command_t;

int verify_command_signature(const command_t *cmd, const uint8_t *key)
{
    uint8_t calculated_hmac[32];
    unsigned int hmac_len;
    
    /* Calculate HMAC over command data */
    HMAC(EVP_sha256(), key, HMAC_KEY_SIZE,
         (unsigned char *)cmd, sizeof(command_t) - 32,
         calculated_hmac, &hmac_len);
    
    /* Compare HMACs */
    if (memcmp(cmd->hmac, calculated_hmac, 32) != 0) {
        return -1; /* Invalid signature */
    }
    
    return 0;
}

int execute_command(const command_t *cmd)
{
    switch (cmd->command_id) {
    case 0x01:
        printf("Executing: REBOOT\n");
        /* system("reboot"); */
        return 0;
    case 0x02:
        printf("Executing: UPDATE_CONFIG\n");
        return 0;
    case 0x03:
        printf("Executing: SEND_STATUS\n");
        return 0;
    default:
        printf("Unknown command: 0x%02x\n", cmd->command_id);
        return -1;
    }
}

int handle_command(const command_t *cmd, const uint8_t *key)
{
    /* Verify signature */
    if (verify_command_signature(cmd, key) != 0) {
        return -1;
    }
    
    /* Execute command */
    return execute_command(cmd);
}

