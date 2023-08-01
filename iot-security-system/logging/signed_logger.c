#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <openssl/hmac.h>
#include <openssl/sha.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>

#define HMAC_KEY_SIZE 32
#define MAX_LOG_SIZE 4096

typedef struct {
    time_t timestamp;
    char level[16];
    char message[MAX_LOG_SIZE];
    uint8_t signature[256];  /* RSA signature */
    uint8_t hmac[32];        /* HMAC for integrity */
} signed_log_entry_t;

int sign_log_entry_hmac(signed_log_entry_t *entry, const uint8_t *key)
{
    unsigned int hmac_len;
    
    /* Calculate HMAC over log entry */
    HMAC(EVP_sha256(), key, HMAC_KEY_SIZE,
         (unsigned char *)entry, sizeof(signed_log_entry_t) - 32,
         entry->hmac, &hmac_len);
    
    return 0;
}

int sign_log_entry_rsa(signed_log_entry_t *entry, RSA *private_key)
{
    unsigned char hash[SHA256_DIGEST_LENGTH];
    unsigned int sig_len = 256;
    
    /* Hash the log entry */
    SHA256((unsigned char *)entry, sizeof(signed_log_entry_t) - 256, hash);
    
    /* Sign the hash */
    if (RSA_sign(NID_sha256, hash, SHA256_DIGEST_LENGTH,
                 entry->signature, &sig_len, private_key) != 1) {
        return -1;
    }
    
    return 0;
}

int write_signed_log(const char *filename, signed_log_entry_t *entry,
                    const uint8_t *hmac_key, RSA *rsa_key)
{
    FILE *fp;
    
    /* Sign with HMAC */
    sign_log_entry_hmac(entry, hmac_key);
    
    /* Sign with RSA if key provided */
    if (rsa_key) {
        sign_log_entry_rsa(entry, rsa_key);
    }
    
    /* Write to file */
    fp = fopen(filename, "ab");
    if (!fp) {
        return -1;
    }
    
    fwrite(entry, sizeof(signed_log_entry_t), 1, fp);
    fclose(fp);
    
    return 0;
}

