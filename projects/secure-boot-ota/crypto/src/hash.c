#include "crypto.h"
#include <mbedtls/sha256.h>
#include <mbedtls/md.h>

// v1.0 - Initial implementation
// SHA-256 and HMAC-SHA256 functions

int crypto_hash_sha256(const uint8_t *data, size_t data_len, uint8_t *hash)
{
    mbedtls_sha256_context ctx;
    int ret;
    
    if (!data || !hash || data_len == 0) {
        return CRYPTO_ERROR_INVALID_PARAM;
    }
    
    mbedtls_sha256_init(&ctx);
    ret = mbedtls_sha256_starts(&ctx, 0); /* 0 = SHA-256, not SHA-224 */
    if (ret == 0) {
        ret = mbedtls_sha256_update(&ctx, data, data_len);
    }
    if (ret == 0) {
        ret = mbedtls_sha256_finish(&ctx, hash);
    }
    mbedtls_sha256_free(&ctx);
    
    // TODO: Could add incremental hashing for large files
    
    if (ret != 0) {
        return CRYPTO_ERROR_INVALID_PARAM;
    }
    
    return CRYPTO_SUCCESS;
}

int crypto_hmac_sha256(const uint8_t *data, size_t data_len,
                      const uint8_t *key, size_t key_len,
                      uint8_t *hmac)
{
    const mbedtls_md_info_t *md_info;
    mbedtls_md_context_t ctx;
    int ret;
    
    if (!data || !key || !hmac || data_len == 0 || key_len == 0) {
        return CRYPTO_ERROR_INVALID_PARAM;
    }
    
    md_info = mbedtls_md_info_from_type(MBEDTLS_MD_SHA256);
    if (!md_info) {
        return CRYPTO_ERROR_INVALID_PARAM;
    }
    
    mbedtls_md_init(&ctx);
    ret = mbedtls_md_setup(&ctx, md_info, 1); /* 1 = HMAC */
    if (ret == 0) {
        ret = mbedtls_md_hmac_starts(&ctx, key, key_len);
    }
    if (ret == 0) {
        ret = mbedtls_md_hmac_update(&ctx, data, data_len);
    }
    if (ret == 0) {
        ret = mbedtls_md_hmac_finish(&ctx, hmac);
    }
    mbedtls_md_free(&ctx);
    
    if (ret != 0) {
        return CRYPTO_ERROR_INVALID_PARAM;
    }
    
    return CRYPTO_SUCCESS;
}

