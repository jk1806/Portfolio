#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <jwt.h>

#define SECRET_KEY "your-secret-key-here"

int generate_jwt_token(const char *device_id, char **token)
{
    jwt_t *jwt;
    time_t now = time(NULL);
    
    jwt_new(&jwt);
    jwt_set_alg(jwt, JWT_ALG_HS256, (unsigned char *)SECRET_KEY, strlen(SECRET_KEY));
    
    jwt_add_grant(jwt, "device_id", device_id);
    jwt_add_grant_int(jwt, "iat", now);
    jwt_add_grant_int(jwt, "exp", now + 3600); /* 1 hour */
    
    *token = jwt_encode_str(jwt);
    jwt_free(jwt);
    
    return 0;
}

int verify_jwt_token(const char *token, char **device_id)
{
    jwt_t *jwt;
    const char *val;
    
    if (jwt_decode(&jwt, token, (unsigned char *)SECRET_KEY, strlen(SECRET_KEY)) != 0) {
        return -1;
    }
    
    val = jwt_get_grant(jwt, "device_id");
    if (val) {
        *device_id = strdup(val);
    }
    
    jwt_free(jwt);
    return 0;
}

