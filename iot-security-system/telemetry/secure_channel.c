#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/ssl.h>
#include <openssl/aes.h>
#include <openssl/evp.h>

int encrypt_telemetry_data(const unsigned char *plaintext, size_t pt_len,
                          const unsigned char *key, const unsigned char *iv,
                          unsigned char *ciphertext, size_t *ct_len)
{
    EVP_CIPHER_CTX *ctx;
    int len;
    int ret;
    
    ctx = EVP_CIPHER_CTX_new();
    if (!ctx) {
        return -1;
    }
    
    if (EVP_EncryptInit_ex(ctx, EVP_aes_256_gcm(), NULL, key, iv) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return -1;
    }
    
    if (EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, pt_len) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return -1;
    }
    *ct_len = len;
    
    if (EVP_EncryptFinal_ex(ctx, ciphertext + len, &len) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return -1;
    }
    *ct_len += len;
    
    EVP_CIPHER_CTX_free(ctx);
    return 0;
}

int send_secure_telemetry(SSL *ssl, const unsigned char *data, size_t data_len,
                         const unsigned char *key, const unsigned char *iv)
{
    unsigned char ciphertext[4096];
    size_t ct_len;
    
    if (encrypt_telemetry_data(data, data_len, key, iv, ciphertext, &ct_len) != 0) {
        return -1;
    }
    
    if (SSL_write(ssl, ciphertext, ct_len) <= 0) {
        return -1;
    }
    
    return 0;
}

