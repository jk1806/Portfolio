#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/ec.h>
#include <openssl/ecdh.h>
#include <openssl/evp.h>
#include <openssl/sha.h>

int derive_session_key(const unsigned char *shared_secret, size_t secret_len,
                      const unsigned char *salt, size_t salt_len,
                      unsigned char *session_key, size_t key_len)
{
    EVP_PKEY_CTX *ctx;
    int ret;
    
    ctx = EVP_PKEY_CTX_new_id(EVP_PKEY_HKDF, NULL);
    if (!ctx) {
        return -1;
    }
    
    if (EVP_PKEY_derive_init(ctx) <= 0) {
        EVP_PKEY_CTX_free(ctx);
        return -1;
    }
    
    if (EVP_PKEY_CTX_set_hkdf_md(ctx, EVP_sha256()) <= 0) {
        EVP_PKEY_CTX_free(ctx);
        return -1;
    }
    
    if (EVP_PKEY_CTX_set1_hkdf_salt(ctx, salt, salt_len) <= 0) {
        EVP_PKEY_CTX_free(ctx);
        return -1;
    }
    
    if (EVP_PKEY_CTX_set1_hkdf_key(ctx, shared_secret, secret_len) <= 0) {
        EVP_PKEY_CTX_free(ctx);
        return -1;
    }
    
    if (EVP_PKEY_derive(ctx, session_key, &key_len) <= 0) {
        EVP_PKEY_CTX_free(ctx);
        return -1;
    }
    
    EVP_PKEY_CTX_free(ctx);
    return 0;
}

