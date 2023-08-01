#include "crypto.h"

/* AES-CTR decrypt (symmetric - same as encrypt) */
int crypto_decrypt_aes_ctr(const uint8_t *ciphertext, size_t ct_len,
                           const uint8_t *key, const uint8_t *iv,
                           uint8_t *plaintext)
{
    /* CTR mode is symmetric - encryption and decryption are the same */
    extern int crypto_encrypt_aes_ctr(const uint8_t *, size_t,
                                      const uint8_t *, const uint8_t *,
                                      uint8_t *);
    return crypto_encrypt_aes_ctr(ciphertext, ct_len, key, iv, plaintext);
}

/* AES-GCM decrypt is implemented in encrypt.c */

