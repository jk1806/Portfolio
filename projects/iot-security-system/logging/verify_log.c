#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/hmac.h>
#include <openssl/sha.h>
#include <openssl/rsa.h>

int verify_log_entry_hmac(const signed_log_entry_t *entry, const uint8_t *key)
{
    uint8_t calculated_hmac[32];
    unsigned int hmac_len;
    
    /* Calculate HMAC */
    HMAC(EVP_sha256(), key, HMAC_KEY_SIZE,
         (unsigned char *)entry, sizeof(signed_log_entry_t) - 32,
         calculated_hmac, &hmac_len);
    
    /* Compare */
    return memcmp(entry->hmac, calculated_hmac, 32) == 0 ? 0 : -1;
}

int verify_log_entry_rsa(const signed_log_entry_t *entry, RSA *public_key)
{
    unsigned char hash[SHA256_DIGEST_LENGTH];
    
    /* Hash the log entry */
    SHA256((unsigned char *)entry, sizeof(signed_log_entry_t) - 256, hash);
    
    /* Verify signature */
    if (RSA_verify(NID_sha256, hash, SHA256_DIGEST_LENGTH,
                   entry->signature, 256, public_key) != 1) {
        return -1;
    }
    
    return 0;
}

