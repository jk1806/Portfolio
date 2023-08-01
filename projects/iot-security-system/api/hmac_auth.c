#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <openssl/hmac.h>
#include <openssl/sha.h>

#define HMAC_KEY_SIZE 32

int generate_hmac_token(const char *device_id, const uint8_t *key, char **token)
{
    char message[256];
    time_t now = time(NULL);
    uint8_t hmac[32];
    unsigned int hmac_len;
    char hex_hmac[65];
    int i;
    
    /* Create message: device_id:timestamp */
    snprintf(message, sizeof(message), "%s:%ld", device_id, now);
    
    /* Calculate HMAC */
    HMAC(EVP_sha256(), key, HMAC_KEY_SIZE,
         (unsigned char *)message, strlen(message),
         hmac, &hmac_len);
    
    /* Convert to hex string */
    for (i = 0; i < 32; i++) {
        sprintf(hex_hmac + i * 2, "%02x", hmac[i]);
    }
    hex_hmac[64] = '\0';
    
    /* Create token: message:hmac */
    *token = malloc(strlen(message) + 1 + 64 + 1);
    sprintf(*token, "%s:%s", message, hex_hmac);
    
    return 0;
}

int verify_hmac_token(const char *token, const uint8_t *key, char **device_id)
{
    char *message_part, *hmac_part;
    char message[256];
    uint8_t calculated_hmac[32], received_hmac[32];
    unsigned int hmac_len;
    int i;
    
    /* Split token */
    message_part = strdup(token);
    hmac_part = strrchr(message_part, ':');
    if (!hmac_part) {
        free(message_part);
        return -1;
    }
    *hmac_part = '\0';
    hmac_part++;
    
    /* Convert hex HMAC to binary */
    for (i = 0; i < 32; i++) {
        sscanf(hmac_part + i * 2, "%2hhx", &received_hmac[i]);
    }
    
    /* Calculate HMAC */
    strncpy(message, message_part, sizeof(message) - 1);
    HMAC(EVP_sha256(), key, HMAC_KEY_SIZE,
         (unsigned char *)message, strlen(message),
         calculated_hmac, &hmac_len);
    
    /* Compare */
    if (memcmp(calculated_hmac, received_hmac, 32) != 0) {
        free(message_part);
        return -1;
    }
    
    /* Extract device_id */
    char *colon = strchr(message, ':');
    if (colon) {
        *colon = '\0';
        *device_id = strdup(message);
    }
    
    free(message_part);
    return 0;
}

