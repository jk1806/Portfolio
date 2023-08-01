#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include <sys/time.h>

// v1.0 - Initial implementation
// Simple rate limiting per client

#define MAX_CLIENTS 1000  // TODO: Use hash table for scalability
#define RATE_LIMIT 100  /* requests per minute - configurable */
#define WINDOW_SIZE 60  /* seconds - sliding window */

typedef struct {
    char client_id[64];
    uint32_t request_count;
    time_t window_start;
} rate_limit_entry_t;

static rate_limit_entry_t rate_limits[MAX_CLIENTS];
static int client_count = 0;

int check_rate_limit(const char *client_id)
{
    time_t now = time(NULL);
    int i;
    
    /* Find existing entry */
    for (i = 0; i < client_count; i++) {
        if (strcmp(rate_limits[i].client_id, client_id) == 0) {
            /* Check if window expired - sliding window */
            if (now - rate_limits[i].window_start > WINDOW_SIZE) {
                /* Reset window - TODO: Could use token bucket instead */
                rate_limits[i].request_count = 0;
                rate_limits[i].window_start = now;
            }
            
            /* Check rate limit */
            if (rate_limits[i].request_count >= RATE_LIMIT) {
                return -1; /* Rate limit exceeded */
            }
            
            rate_limits[i].request_count++;
            return 0; /* Allowed */
        }
    }
    
    /* Add new client */
    if (client_count < MAX_CLIENTS) {
        strncpy(rate_limits[client_count].client_id, client_id,
                sizeof(rate_limits[client_count].client_id) - 1);
        rate_limits[client_count].request_count = 1;
        rate_limits[client_count].window_start = now;
        client_count++;
        return 0;
    }
    
    return -1; /* Too many clients */
}

