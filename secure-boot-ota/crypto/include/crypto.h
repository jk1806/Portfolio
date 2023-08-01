#ifndef CRYPTO_H
#define CRYPTO_H

#include <stdint.h>
#include <stddef.h>

#define RSA_KEY_SIZE_2048 256
#define ECDSA_KEY_SIZE_P256 64
#define AES_KEY_SIZE_256 32
#define SHA256_HASH_SIZE 32
#define HMAC_SIZE 32

/* Signature verification result */
typedef enum {
    CRYPTO_SUCCESS = 0,
    CRYPTO_ERROR_INVALID_SIGNATURE = -1,
    CRYPTO_ERROR_INVALID_KEY = -2,
    CRYPTO_ERROR_BUFFER_TOO_SMALL = -3,
    CRYPTO_ERROR_INVALID_PARAM = -4
} crypto_result_t;

/* Initialize crypto library */
int crypto_init(void);

/* Signature Operations */
int crypto_sign_rsa(const uint8_t *data, size_t data_len,
                    const uint8_t *private_key, size_t key_len,
                    uint8_t *signature, size_t *sig_len);

int crypto_verify_rsa(const uint8_t *data, size_t data_len,
                      const uint8_t *signature, size_t sig_len,
                      const uint8_t *public_key, size_t key_len);

int crypto_sign_ecdsa(const uint8_t *data, size_t data_len,
                      const uint8_t *private_key, size_t key_len,
                      uint8_t *signature, size_t *sig_len);

int crypto_verify_ecdsa(const uint8_t *data, size_t data_len,
                        const uint8_t *signature, size_t sig_len,
                        const uint8_t *public_key, size_t key_len);

/* Encryption Operations */
int crypto_encrypt_aes_ctr(const uint8_t *plaintext, size_t pt_len,
                           const uint8_t *key, const uint8_t *iv,
                           uint8_t *ciphertext);

int crypto_decrypt_aes_ctr(const uint8_t *ciphertext, size_t ct_len,
                           const uint8_t *key, const uint8_t *iv,
                           uint8_t *plaintext);

int crypto_encrypt_aes_gcm(const uint8_t *plaintext, size_t pt_len,
                          const uint8_t *key, const uint8_t *iv,
                          const uint8_t *aad, size_t aad_len,
                          uint8_t *ciphertext, uint8_t *tag);

int crypto_decrypt_aes_gcm(const uint8_t *ciphertext, size_t ct_len,
                           const uint8_t *key, const uint8_t *iv,
                           const uint8_t *aad, size_t aad_len,
                           const uint8_t *tag,
                           uint8_t *plaintext);

/* Hash Operations */
int crypto_hash_sha256(const uint8_t *data, size_t data_len,
                       uint8_t *hash);

/* HMAC Operations */
int crypto_hmac_sha256(const uint8_t *data, size_t data_len,
                       const uint8_t *key, size_t key_len,
                       uint8_t *hmac);

/* Key Derivation */
int crypto_hkdf(const uint8_t *salt, size_t salt_len,
                const uint8_t *ikm, size_t ikm_len,
                const uint8_t *info, size_t info_len,
                uint8_t *okm, size_t okm_len);

int crypto_pbkdf2(const uint8_t *password, size_t pwd_len,
                 const uint8_t *salt, size_t salt_len,
                 uint32_t iterations,
                 uint8_t *key, size_t key_len);

#endif /* CRYPTO_H */

