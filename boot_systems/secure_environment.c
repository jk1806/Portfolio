/**
 * Secure Environment Implementation
 * Author: jk1806
 * Created: 2024-05-18
 * 
 * Secure environment for storing sensitive configuration
 * Protected from tampering and unauthorized access
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/crypto.h>
#include <linux/string.h>

#define SECURE_ENV_VERSION "1.0.0"
#define MAX_ENV_VARS 64
#define MAX_VAR_NAME 32
#define MAX_VAR_VALUE 256

struct secure_env_var {
    char name[MAX_VAR_NAME];
    char value[MAX_VAR_VALUE];
    bool encrypted;
    u32 flags;
};

static struct secure_env_var secure_env[MAX_ENV_VARS];
static int env_count = 0;
static bool env_locked = false;
static u8 env_key[32];  // AES-256 key

/**
 * Initialize secure environment
 */
int secure_env_init(void)
{
    int ret;
    
    // Generate or load encryption key
    ret = get_random_bytes(env_key, sizeof(env_key));
    if (ret) {
        pr_err("Secure Env: Failed to generate key\n");
        return ret;
    }
    
    pr_info("Secure Env: Initialized\n");
    
    return 0;
}

/**
 * Set secure environment variable
 */
int secure_env_set(const char *name, const char *value, bool encrypt)
{
    int i;
    struct crypto_cipher *tfm;
    u8 encrypted_value[MAX_VAR_VALUE];
    int ret;
    
    if (env_locked) {
        pr_err("Secure Env: Environment is locked\n");
        return -EACCES;
    }
    
    if (strlen(name) >= MAX_VAR_NAME) {
        return -EINVAL;
    }
    
    if (strlen(value) >= MAX_VAR_VALUE) {
        return -EINVAL;
    }
    
    // Find existing or free slot
    for (i = 0; i < env_count; i++) {
        if (strcmp(secure_env[i].name, name) == 0) {
            break;
        }
    }
    
    if (i >= MAX_ENV_VARS) {
        pr_err("Secure Env: Maximum variables reached\n");
        return -ENOSPC;
    }
    
    if (i == env_count) {
        env_count++;
    }
    
    strncpy(secure_env[i].name, name, MAX_VAR_NAME - 1);
    secure_env[i].name[MAX_VAR_NAME - 1] = '\0';
    
    if (encrypt) {
        // Encrypt value
        tfm = crypto_alloc_cipher("aes", 0, 0);
        if (!tfm) {
            return -ENOMEM;
        }
        
        ret = crypto_cipher_setkey(tfm, env_key, sizeof(env_key));
        if (ret) {
            crypto_free_cipher(tfm);
            return ret;
        }
        
        crypto_cipher_encrypt_one(tfm, encrypted_value, value);
        crypto_free_cipher(tfm);
        
        memcpy(secure_env[i].value, encrypted_value, sizeof(encrypted_value));
        secure_env[i].encrypted = true;
    } else {
        strncpy(secure_env[i].value, value, MAX_VAR_VALUE - 1);
        secure_env[i].value[MAX_VAR_VALUE - 1] = '\0';
        secure_env[i].encrypted = false;
    }
    
    pr_info("Secure Env: Set %s\n", name);
    
    return 0;
}

/**
 * Get secure environment variable
 */
int secure_env_get(const char *name, char *value, size_t value_size)
{
    int i;
    struct crypto_cipher *tfm;
    u8 decrypted_value[MAX_VAR_VALUE];
    int ret;
    
    for (i = 0; i < env_count; i++) {
        if (strcmp(secure_env[i].name, name) == 0) {
            break;
        }
    }
    
    if (i >= env_count) {
        return -ENOENT;
    }
    
    if (secure_env[i].encrypted) {
        // Decrypt value
        tfm = crypto_alloc_cipher("aes", 0, 0);
        if (!tfm) {
            return -ENOMEM;
        }
        
        ret = crypto_cipher_setkey(tfm, env_key, sizeof(env_key));
        if (ret) {
            crypto_free_cipher(tfm);
            return ret;
        }
        
        crypto_cipher_decrypt_one(tfm, decrypted_value, secure_env[i].value);
        crypto_free_cipher(tfm);
        
        strncpy(value, (char *)decrypted_value, value_size - 1);
        value[value_size - 1] = '\0';
    } else {
        strncpy(value, secure_env[i].value, value_size - 1);
        value[value_size - 1] = '\0';
    }
    
    return 0;
}

/**
 * Lock secure environment
 */
int secure_env_lock(void)
{
    if (env_locked) {
        return 0;
    }
    
    // Clear key from memory (if possible)
    memset(env_key, 0, sizeof(env_key));
    
    env_locked = true;
    
    pr_info("Secure Env: Environment locked\n");
    
    return 0;
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("jk1806");
MODULE_DESCRIPTION("Secure Environment Management");
MODULE_VERSION(SECURE_ENV_VERSION);

