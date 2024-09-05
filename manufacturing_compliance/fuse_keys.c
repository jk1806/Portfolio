/**
 * Fuse Keys Management
 * Author: jk1806
 * Created: 2024-08-10
 * 
 * Manufacturing fuse key programming
 * Handles device-specific key provisioning during production
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/crypto.h>

#define FUSE_KEY_VERSION "1.0.0"
#define MAX_FUSE_KEYS 16

struct fuse_key {
    char name[32];
    u8 key[32];
    size_t key_len;
    bool programmed;
    u32 usage_flags;
};

static struct fuse_key fuse_keys[MAX_FUSE_KEYS];
static int fuse_key_count = 0;

/**
 * Program fuse key
 */
int fuse_key_program(const char *name, const u8 *key_data, size_t key_len)
{
    int i;
    
    if (fuse_key_count >= MAX_FUSE_KEYS) {
        pr_err("Fuse Key: Maximum keys reached\n");
        return -ENOSPC;
    }
    
    if (key_len > 32) {
        pr_err("Fuse Key: Key too large\n");
        return -EINVAL;
    }
    
    // Check if already exists
    for (i = 0; i < fuse_key_count; i++) {
        if (strcmp(fuse_keys[i].name, name) == 0) {
            if (fuse_keys[i].programmed) {
                pr_warn("Fuse Key: '%s' already programmed\n", name);
                return -EEXIST;
            }
            break;
        }
    }
    
    if (i == fuse_key_count) {
        fuse_key_count++;
    }
    
    strncpy(fuse_keys[i].name, name, sizeof(fuse_keys[i].name) - 1);
    memcpy(fuse_keys[i].key, key_data, key_len);
    fuse_keys[i].key_len = key_len;
    fuse_keys[i].programmed = true;
    
    pr_info("Fuse Key: Programmed '%s', len: %zu\n", name, key_len);
    
    return 0;
}

/**
 * Get fuse key
 */
int fuse_key_get(const char *name, u8 *key_data, size_t *key_len)
{
    int i;
    
    for (i = 0; i < fuse_key_count; i++) {
        if (strcmp(fuse_keys[i].name, name) == 0) {
            if (!fuse_keys[i].programmed) {
                return -ENOENT;
            }
            
            if (key_data && key_len) {
                size_t copy_len = min_t(size_t, fuse_keys[i].key_len, *key_len);
                memcpy(key_data, fuse_keys[i].key, copy_len);
                *key_len = copy_len;
            }
            
            return 0;
        }
    }
    
    return -ENOENT;
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("jk1806");
MODULE_DESCRIPTION("Fuse Keys Management");
MODULE_VERSION(FUSE_KEY_VERSION);

