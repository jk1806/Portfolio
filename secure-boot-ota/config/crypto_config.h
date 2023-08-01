#ifndef CRYPTO_CONFIG_H
#define CRYPTO_CONFIG_H

/* Crypto algorithm selection */
#define USE_RSA_2048 1
#define USE_ECDSA_P256 0
#define USE_AES_256_GCM 1
#define USE_AES_256_CTR 0

/* Key sizes */
#define RSA_KEY_SIZE 2048
#define ECDSA_KEY_SIZE 256
#define AES_KEY_SIZE 256

/* Hash algorithm */
#define HASH_ALGORITHM_SHA256 1

/* HMAC */
#define HMAC_KEY_SIZE 32

/* Key derivation */
#define HKDF_SALT_SIZE 32
#define PBKDF2_ITERATIONS 10000

#endif /* CRYPTO_CONFIG_H */

