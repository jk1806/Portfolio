#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "crypto.h"

// Basic crypto tests - TODO: Add more edge cases

void test_hash_sha256(void)
{
    uint8_t data[] = "Hello, World!";
    uint8_t hash[SHA256_HASH_SIZE];
    
    printf("Testing SHA-256...\n");
    
    assert(crypto_hash_sha256(data, strlen((char *)data), hash) == CRYPTO_SUCCESS);
    
    /* Verify hash is not all zeros - basic sanity check */
    int all_zero = 1;
    for (int i = 0; i < SHA256_HASH_SIZE; i++) {
        if (hash[i] != 0) {
            all_zero = 0;
            break;
        }
    }
    assert(all_zero == 0);
    // TODO: Add known-answer test vectors
    
    printf("SHA-256 test PASSED\n");
}

void test_hmac_sha256(void)
{
    uint8_t data[] = "Test data";
    uint8_t key[] = "secret key";
    uint8_t hmac[HMAC_SIZE];
    
    printf("Testing HMAC-SHA256...\n");
    
    assert(crypto_hmac_sha256(data, strlen((char *)data),
                              key, strlen((char *)key),
                              hmac) == CRYPTO_SUCCESS);
    
    printf("HMAC-SHA256 test PASSED\n");
}

int main(void)
{
    printf("=== Crypto Tests ===\n");
    
    assert(crypto_init() == CRYPTO_SUCCESS);
    
    test_hash_sha256();
    test_hmac_sha256();
    
    printf("\nAll crypto tests completed\n");
    return 0;
}

