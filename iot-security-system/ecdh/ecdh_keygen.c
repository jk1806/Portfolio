#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/ec.h>
#include <openssl/ecdh.h>
#include <openssl/pem.h>
#include <openssl/evp.h>

#define CURVE_NAME "prime256v1"

int generate_ecdh_keypair(EVP_PKEY **keypair)
{
    EVP_PKEY_CTX *ctx;
    EVP_PKEY *key = NULL;
    
    ctx = EVP_PKEY_CTX_new_id(EVP_PKEY_EC, NULL);
    if (!ctx) {
        return -1;
    }
    
    if (EVP_PKEY_keygen_init(ctx) <= 0) {
        EVP_PKEY_CTX_free(ctx);
        return -1;
    }
    
    if (EVP_PKEY_CTX_set_ec_paramgen_curve_nid(ctx, NID_X9_62_prime256v1) <= 0) {
        EVP_PKEY_CTX_free(ctx);
        return -1;
    }
    
    if (EVP_PKEY_keygen(ctx, &key) <= 0) {
        EVP_PKEY_CTX_free(ctx);
        return -1;
    }
    
    EVP_PKEY_CTX_free(ctx);
    *keypair = key;
    return 0;
}

int derive_shared_secret(EVP_PKEY *priv_key, EVP_PKEY *peer_pub_key,
                        unsigned char **secret, size_t *secret_len)
{
    EVP_PKEY_CTX *ctx;
    size_t len;
    
    ctx = EVP_PKEY_CTX_new(priv_key, NULL);
    if (!ctx) {
        return -1;
    }
    
    if (EVP_PKEY_derive_init(ctx) <= 0) {
        EVP_PKEY_CTX_free(ctx);
        return -1;
    }
    
    if (EVP_PKEY_derive_set_peer(ctx, peer_pub_key) <= 0) {
        EVP_PKEY_CTX_free(ctx);
        return -1;
    }
    
    if (EVP_PKEY_derive(ctx, NULL, &len) <= 0) {
        EVP_PKEY_CTX_free(ctx);
        return -1;
    }
    
    *secret = malloc(len);
    if (!*secret) {
        EVP_PKEY_CTX_free(ctx);
        return -1;
    }
    
    if (EVP_PKEY_derive(ctx, *secret, &len) <= 0) {
        free(*secret);
        EVP_PKEY_CTX_free(ctx);
        return -1;
    }
    
    *secret_len = len;
    EVP_PKEY_CTX_free(ctx);
    return 0;
}

int main(void)
{
    EVP_PKEY *keypair1, *keypair2;
    unsigned char *secret1, *secret2;
    size_t secret_len1, secret_len2;
    
    printf("Generating ECDH keypairs...\n");
    
    if (generate_ecdh_keypair(&keypair1) != 0 ||
        generate_ecdh_keypair(&keypair2) != 0) {
        fprintf(stderr, "Failed to generate keypairs\n");
        return 1;
    }
    
    printf("Deriving shared secrets...\n");
    
    if (derive_shared_secret(keypair1, keypair2, &secret1, &secret_len1) != 0 ||
        derive_shared_secret(keypair2, keypair1, &secret2, &secret_len2) != 0) {
        fprintf(stderr, "Failed to derive shared secrets\n");
        return 1;
    }
    
    if (secret_len1 == secret_len2 &&
        memcmp(secret1, secret2, secret_len1) == 0) {
        printf("ECDH key exchange successful!\n");
        printf("Shared secret length: %zu bytes\n", secret_len1);
    } else {
        printf("ECDH key exchange failed\n");
    }
    
    free(secret1);
    free(secret2);
    EVP_PKEY_free(keypair1);
    EVP_PKEY_free(keypair2);
    
    return 0;
}

