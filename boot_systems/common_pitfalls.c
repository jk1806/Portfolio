/**
 * Common Boot Pitfalls and Mitigations
 * Author: jk1806
 * Created: 2024-08-05
 * 
 * Addresses common boot security pitfalls
 * Unsigned DTB, environment tampering, missing anti-rollback
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/of_fdt.h>

#define PITFALLS_VERSION "1.0.0"

/**
 * Check for unsigned DTB
 */
int boot_check_dtb_signature(void *dtb_base)
{
    const void *sig;
    int sig_len;
    
    if (!dtb_base) {
        pr_err("Pitfalls: DTB base is NULL\n");
        return -1;
    }
    
    sig = fdt_getprop(dtb_base, 0, "signature", &sig_len);
    if (!sig) {
        pr_warn("Pitfalls: DTB is unsigned - security risk!\n");
        return -1;
    }
    
    // Verify signature
    if (dtb_verify_signature(dtb_base) < 0) {
        pr_err("Pitfalls: DTB signature verification failed\n");
        return -1;
    }
    
    pr_info("Pitfalls: DTB signature verified\n");
    
    return 0;
}

/**
 * Detect environment tampering
 */
int boot_check_env_tamper(void)
{
    char *env_hash;
    char *stored_hash;
    u8 calculated_hash[32];
    
    // Get stored hash
    stored_hash = env_get("env_hash");
    if (!stored_hash) {
        pr_warn("Pitfalls: No environment hash stored\n");
        return -1;
    }
    
    // Calculate current hash
    env_calculate_hash(calculated_hash);
    
    // Compare
    if (memcmp(stored_hash, calculated_hash, 32) != 0) {
        pr_err("Pitfalls: Environment tampering detected!\n");
        return -1;
    }
    
    pr_info("Pitfalls: Environment integrity verified\n");
    
    return 0;
}

/**
 * Check anti-rollback protection
 */
int boot_check_anti_rollback(void)
{
    u32 current_version;
    u32 min_version;
    
    current_version = boot_get_version();
    min_version = boot_get_min_version();
    
    if (current_version < min_version) {
        pr_err("Pitfalls: Version rollback detected! Current: %d, Min: %d\n",
               current_version, min_version);
        return -1;
    }
    
    pr_info("Pitfalls: Anti-rollback check passed (version %d)\n", current_version);
    
    return 0;
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("jk1806");
MODULE_DESCRIPTION("Common Boot Pitfalls Mitigation");
MODULE_VERSION(PITFALLS_VERSION);

