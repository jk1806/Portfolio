/**
 * eFuses and OTP (One-Time Programmable) Memory
 * Author: jk1806
 * Created: 2024-05-10
 * 
 * eFuse and OTP memory management
 * Handles secure key storage and device provisioning
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/io.h>
#include <linux/mutex.h>

#define EFUSE_VERSION "1.0.0"
#define EFUSE_MAX_BITS 2048
#define EFUSE_WORD_SIZE 32

struct efuse_region {
    u32 base_addr;
    u32 size_bits;
    u32 *data;
    bool locked;
    struct mutex lock;
};

static struct efuse_region efuse_regions[4];
static int efuse_region_count;

/**
 * Read eFuse bit
 */
int efuse_read_bit(int region, int bit)
{
    u32 word;
    u32 bit_mask;
    int word_idx;
    
    if (region >= efuse_region_count) {
        return -EINVAL;
    }
    
    if (bit >= efuse_regions[region].size_bits) {
        return -EINVAL;
    }
    
    word_idx = bit / EFUSE_WORD_SIZE;
    bit_mask = 1 << (bit % EFUSE_WORD_SIZE);
    
    word = readl(efuse_regions[region].base_addr + word_idx * 4);
    
    return (word & bit_mask) ? 1 : 0;
}

/**
 * Write eFuse bit (one-time operation)
 */
int efuse_write_bit(int region, int bit)
{
    u32 word;
    u32 bit_mask;
    int word_idx;
    int ret;
    
    if (region >= efuse_region_count) {
        return -EINVAL;
    }
    
    mutex_lock(&efuse_regions[region].lock);
    
    // Check if already programmed
    ret = efuse_read_bit(region, bit);
    if (ret < 0) {
        mutex_unlock(&efuse_regions[region].lock);
        return ret;
    }
    
    if (ret == 1) {
        pr_warn("eFuse: Bit %d already programmed\n", bit);
        mutex_unlock(&efuse_regions[region].lock);
        return 0;
    }
    
    // Check if region is locked
    if (efuse_regions[region].locked) {
        pr_err("eFuse: Region %d is locked\n", region);
        mutex_unlock(&efuse_regions[region].lock);
        return -EACCES;
    }
    
    word_idx = bit / EFUSE_WORD_SIZE;
    bit_mask = 1 << (bit % EFUSE_WORD_SIZE);
    
    word = readl(efuse_regions[region].base_addr + word_idx * 4);
    word |= bit_mask;
    
    // Program eFuse (hardware-specific)
    writel(word, efuse_regions[region].base_addr + word_idx * 4);
    
    // Wait for programming to complete
    udelay(100);
    
    // Verify
    word = readl(efuse_regions[region].base_addr + word_idx * 4);
    if (!(word & bit_mask)) {
        pr_err("eFuse: Programming failed\n");
        mutex_unlock(&efuse_regions[region].lock);
        return -EIO;
    }
    
    mutex_unlock(&efuse_regions[region].lock);
    
    pr_info("eFuse: Bit %d programmed successfully\n", bit);
    
    return 0;
}

/**
 * Read eFuse key
 */
int efuse_read_key(int region, u8 *key, size_t key_len)
{
    int i;
    int bit;
    u8 byte = 0;
    
    if (key_len * 8 > efuse_regions[region].size_bits) {
        return -EINVAL;
    }
    
    for (i = 0; i < key_len * 8; i++) {
        bit = efuse_read_bit(region, i);
        if (bit < 0) {
            return bit;
        }
        
        if (bit) {
            byte |= (1 << (i % 8));
        }
        
        if ((i + 1) % 8 == 0) {
            key[i / 8] = byte;
            byte = 0;
        }
    }
    
    return 0;
}

/**
 * Lock eFuse region
 */
int efuse_lock_region(int region)
{
    if (region >= efuse_region_count) {
        return -EINVAL;
    }
    
    mutex_lock(&efuse_regions[region].lock);
    
    if (efuse_regions[region].locked) {
        mutex_unlock(&efuse_regions[region].lock);
        return 0;  // Already locked
    }
    
    // Set lock bit (hardware-specific)
    writel(0x1, efuse_regions[region].base_addr + 0x100);  // Lock register
    
    efuse_regions[region].locked = true;
    
    mutex_unlock(&efuse_regions[region].lock);
    
    pr_info("eFuse: Region %d locked\n", region);
    
    return 0;
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("jk1806");
MODULE_DESCRIPTION("eFuse and OTP Memory Management");
MODULE_VERSION(EFUSE_VERSION);

