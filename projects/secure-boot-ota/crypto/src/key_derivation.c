#include "crypto.h"
#include <mbedtls/hkdf.h>
#include <mbedtls/pkcs5.h>
#include <mbedtls/md.h>
#include <string.h>

// v1.0 - Initial implementation
// HKDF and PBKDF2 key derivation functions

int crypto_hkdf(const uint8_t *salt, size_t salt_len,
                const uint8_t *ikm, size_t ikm_len,
                const uint8_t *info, size_t info_len,
                uint8_t *okm, size_t okm_len)
{
    const mbedtls_md_info_t *md_info;
    int ret;
    
    if (!salt || !ikm || !okm || ikm_len == 0 || okm_len == 0) {
        return CRYPTO_ERROR_INVALID_PARAM;
    }
    
    md_info = mbedtls_md_info_from_type(MBEDTLS_MD_SHA256);
    if (!md_info) {
        return CRYPTO_ERROR_INVALID_PARAM;
    }
    
    ret = mbedtls_hkdf(md_info, salt, salt_len, ikm, ikm_len,
                       info, info_len, okm, okm_len);
    
    if (ret != 0) {
        return CRYPTO_ERROR_INVALID_PARAM;
    }
    
    return CRYPTO_SUCCESS;
}

int crypto_pbkdf2(const uint8_t *password, size_t pwd_len,
                  const uint8_t *salt, size_t salt_len,
                  uint32_t iterations,
                  uint8_t *key, size_t key_len)
{
    const mbedtls_md_info_t *md_info;
    int ret;
    
    if (!password || !salt || !key || pwd_len == 0 || key_len == 0) {
        return CRYPTO_ERROR_INVALID_PARAM;
    }
    
    md_info = mbedtls_md_info_from_type(MBEDTLS_MD_SHA256);
    if (!md_info) {
        return CRYPTO_ERROR_INVALID_PARAM;
    }
    
    ret = mbedtls_pkcs5_pbkdf2_hmac(md_info, password, pwd_len,
                                    salt, salt_len, iterations,
                                    key_len, key);
    
    if (ret != 0) {
        return CRYPTO_ERROR_INVALID_PARAM;
    }
    
    return CRYPTO_SUCCESS;
}

int crypto_init(void)
{
    /* Initialize any required crypto state */
    // TODO: Add entropy source initialization if needed
    // TODO: Seed RNG if required
    return CRYPTO_SUCCESS;
}

