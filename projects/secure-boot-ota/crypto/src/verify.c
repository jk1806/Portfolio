#include "crypto.h"
#include "boot_config.h"
#include <mbedtls/rsa.h>
#include <mbedtls/pk.h>
#include <mbedtls/sha256.h>
#include <mbedtls/error.h>
#include <string.h>

// v1.1 - Added ECDSA support (2024-03-20)
// v1.0 - Initial RSA verification

static int load_public_key(const uint8_t *key_data, size_t key_len, mbedtls_pk_context *pk)
{
    int ret;
    mbedtls_pk_init(pk);
    
#if BOOT_SIGNATURE_ALGORITHM == BOOT_SIGNATURE_ALGORITHM_RSA_2048
    ret = mbedtls_pk_parse_public_key(pk, key_data, key_len);
#elif BOOT_SIGNATURE_ALGORITHM == BOOT_SIGNATURE_ALGORITHM_ECDSA_P256
    ret = mbedtls_pk_parse_public_key(pk, key_data, key_len);
#else
    ret = MBEDTLS_ERR_PK_FEATURE_UNAVAILABLE;
#endif
    
    if (ret != 0) {
        mbedtls_pk_free(pk);
        return CRYPTO_ERROR_INVALID_KEY;
    }
    
    return CRYPTO_SUCCESS;
}

int crypto_verify_rsa(const uint8_t *data, size_t data_len,
                      const uint8_t *signature, size_t sig_len,
                      const uint8_t *public_key, size_t key_len)
{
    int ret;
    mbedtls_pk_context pk;
    uint8_t hash[SHA256_HASH_SIZE];
    mbedtls_md_context_t md_ctx;
    const mbedtls_md_info_t *md_info;
    
    if (!data || !signature || !public_key || data_len == 0) {
        return CRYPTO_ERROR_INVALID_PARAM;
    }
    
    /* Load public key - supports both RSA and ECDSA */
    ret = load_public_key(public_key, key_len, &pk);
    if (ret != CRYPTO_SUCCESS) {
        // FIXME: Should log which key type failed
        return ret;
    }
    
    /* Hash the data - SHA256 for both RSA and ECDSA */
    md_info = mbedtls_md_info_from_type(MBEDTLS_MD_SHA256);
    if (!md_info) {
        mbedtls_pk_free(&pk);
        return CRYPTO_ERROR_INVALID_PARAM;
    }
    
    mbedtls_md_init(&md_ctx);
    ret = mbedtls_md_setup(&md_ctx, md_info, 0);
    if (ret != 0) {
        mbedtls_pk_free(&pk);
        return CRYPTO_ERROR_INVALID_PARAM;
    }
    
    ret = mbedtls_md_starts(&md_ctx);
    if (ret == 0) {
        ret = mbedtls_md_update(&md_ctx, data, data_len);
    }
    if (ret == 0) {
        ret = mbedtls_md_finish(&md_ctx, hash);
    }
    mbedtls_md_free(&md_ctx);
    
    if (ret != 0) {
        mbedtls_pk_free(&pk);
        return CRYPTO_ERROR_INVALID_PARAM;
    }
    
    /* Verify signature */
    ret = mbedtls_pk_verify(&pk, MBEDTLS_MD_SHA256, hash, SHA256_HASH_SIZE,
                           signature, sig_len);
    
    mbedtls_pk_free(&pk);
    
    if (ret != 0) {
        return CRYPTO_ERROR_INVALID_SIGNATURE;
    }
    
    return CRYPTO_SUCCESS;
}

int crypto_verify_ecdsa(const uint8_t *data, size_t data_len,
                       const uint8_t *signature, size_t sig_len,
                       const uint8_t *public_key, size_t key_len)
{
    int ret;
    mbedtls_pk_context pk;
    uint8_t hash[SHA256_HASH_SIZE];
    mbedtls_md_context_t md_ctx;
    const mbedtls_md_info_t *md_info;
    
    if (!data || !signature || !public_key || data_len == 0) {
        return CRYPTO_ERROR_INVALID_PARAM;
    }
    
    /* Load public key */
    ret = load_public_key(public_key, key_len, &pk);
    if (ret != CRYPTO_SUCCESS) {
        return ret;
    }
    
    /* Hash the data */
    md_info = mbedtls_md_info_from_type(MBEDTLS_MD_SHA256);
    if (!md_info) {
        mbedtls_pk_free(&pk);
        return CRYPTO_ERROR_INVALID_PARAM;
    }
    
    mbedtls_md_init(&md_ctx);
    ret = mbedtls_md_setup(&md_ctx, md_info, 0);
    if (ret != 0) {
        mbedtls_pk_free(&pk);
        return CRYPTO_ERROR_INVALID_PARAM;
    }
    
    ret = mbedtls_md_starts(&md_ctx);
    if (ret == 0) {
        ret = mbedtls_md_update(&md_ctx, data, data_len);
    }
    if (ret == 0) {
        ret = mbedtls_md_finish(&md_ctx, hash);
    }
    mbedtls_md_free(&md_ctx);
    
    if (ret != 0) {
        mbedtls_pk_free(&pk);
        return CRYPTO_ERROR_INVALID_PARAM;
    }
    
    /* Verify signature */
    ret = mbedtls_pk_verify(&pk, MBEDTLS_MD_SHA256, hash, SHA256_HASH_SIZE,
                           signature, sig_len);
    
    mbedtls_pk_free(&pk);
    
    if (ret != 0) {
        return CRYPTO_ERROR_INVALID_SIGNATURE;
    }
    
    return CRYPTO_SUCCESS;
}

