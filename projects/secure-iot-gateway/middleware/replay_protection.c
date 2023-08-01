#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include <openssl/sha.h>

// v1.0 - Initial implementation
// Replay protection using nonce cache

#define MAX_NONCES 1000  // TODO: Make configurable
#define NONCE_TIMEOUT 300  /* 5 minutes - might need tuning */
    uint8_t nonce[32];
    time_t timestamp;
} nonce_entry_t;

static nonce_entry_t nonce_cache[MAX_NONCES];
static int nonce_count = 0;

int check_replay(uint8_t *nonce, size_t nonce_len)
{
    time_t now = time(NULL);
    int i;
    
    /* Check if nonce exists in cache */
    for (i = 0; i < nonce_count; i++) {
        if (memcmp(nonce_cache[i].nonce, nonce, nonce_len) == 0) {
            /* Replay detected */
            return -1;
        }
        
        /* Remove expired entries - linear search, could be optimized */
        if (now - nonce_cache[i].timestamp > NONCE_TIMEOUT) {
            memmove(&nonce_cache[i], &nonce_cache[i + 1],
                   (nonce_count - i - 1) * sizeof(nonce_entry_t));
            nonce_count--;
            i--;  // Check same index again after removal
        }
    }
    
    /* Add new nonce */
    if (nonce_count < MAX_NONCES) {
        memcpy(nonce_cache[nonce_count].nonce, nonce, nonce_len);
        nonce_cache[nonce_count].timestamp = now;
        nonce_count++;
    }
    
    return 0;
}

