/**
 * Device Tree Blob (DTB) Implementation
 * Author: jk1806
 * Created: 2024-03-20
 * 
 * Device Tree parsing and manipulation
 * Supports FDT format parsing and node/property access
 */

#include <linux/of.h>
#include <linux/of_fdt.h>
#include <linux/libfdt.h>
#include <linux/slab.h>

#define DTB_VERSION "1.0.0"
#define MAX_DTB_SIZE (512 * 1024)  // 512KB

static void *dtb_base;
static size_t dtb_size;

/**
 * Initialize DTB from memory
 */
int dtb_init_from_memory(void *base, size_t size)
{
    int ret;
    
    if (!base || size == 0) {
        pr_err("DTB: Invalid parameters\n");
        return -EINVAL;
    }
    
    if (size > MAX_DTB_SIZE) {
        pr_err("DTB: Size too large: %zu\n", size);
        return -EINVAL;
    }
    
    // Verify FDT magic
    if (fdt_check_header(base)) {
        pr_err("DTB: Invalid FDT header\n");
        return -EINVAL;
    }
    
    dtb_base = base;
    dtb_size = size;
    
    pr_info("DTB: Initialized, size: %zu bytes\n", size);
    
    return 0;
}

/**
 * Get property value
 */
int dtb_get_property(const char *node_path, const char *prop_name,
                     void *value, size_t *len)
{
    int node_offset;
    const void *prop;
    int prop_len;
    
    if (!dtb_base) {
        pr_err("DTB: Not initialized\n");
        return -EINVAL;
    }
    
    // Find node
    node_offset = fdt_path_offset(dtb_base, node_path);
    if (node_offset < 0) {
        pr_err("DTB: Node not found: %s\n", node_path);
        return -ENOENT;
    }
    
    // Get property
    prop = fdt_getprop(dtb_base, node_offset, prop_name, &prop_len);
    if (!prop) {
        pr_err("DTB: Property not found: %s\n", prop_name);
        return -ENOENT;
    }
    
    // Copy value
    if (value && len) {
        size_t copy_len = min_t(size_t, prop_len, *len);
        memcpy(value, prop, copy_len);
        *len = copy_len;
    }
    
    pr_debug("DTB: Got property %s/%s, len: %d\n",
             node_path, prop_name, prop_len);
    
    return 0;
}

/**
 * Set property value
 */
int dtb_set_property(const char *node_path, const char *prop_name,
                     const void *value, size_t len)
{
    int node_offset;
    int ret;
    
    if (!dtb_base) {
        pr_err("DTB: Not initialized\n");
        return -EINVAL;
    }
    
    // Find or create node
    node_offset = fdt_path_offset(dtb_base, node_path);
    if (node_offset < 0) {
        // Create node
        node_offset = fdt_add_subnode(dtb_base, 0, node_path);
        if (node_offset < 0) {
            pr_err("DTB: Failed to create node: %s\n", node_path);
            return node_offset;
        }
    }
    
    // Set property
    ret = fdt_setprop(dtb_base, node_offset, prop_name, value, len);
    if (ret) {
        pr_err("DTB: Failed to set property: %s\n", prop_name);
        return ret;
    }
    
    pr_info("DTB: Set property %s/%s, len: %zu\n",
            node_path, prop_name, len);
    
    return 0;
}

/**
 * Get compatible strings
 */
int dtb_get_compatible(const char *node_path, char **compatibles, int *count)
{
    int node_offset;
    const char *compat;
    int compat_len;
    int i;
    
    if (!dtb_base) {
        return -EINVAL;
    }
    
    node_offset = fdt_path_offset(dtb_base, node_path);
    if (node_offset < 0) {
        return -ENOENT;
    }
    
    compat = fdt_getprop(dtb_base, node_offset, "compatible", &compat_len);
    if (!compat) {
        *count = 0;
        return 0;
    }
    
    // Count compatible strings
    *count = 0;
    for (i = 0; i < compat_len; i++) {
        if (compat[i] == '\0') {
            (*count)++;
        }
    }
    
    // Allocate and copy
    *compatibles = kmalloc(compat_len, GFP_KERNEL);
    if (!*compatibles) {
        return -ENOMEM;
    }
    
    memcpy(*compatibles, compat, compat_len);
    
    return 0;
}

/**
 * Verify DTB signature
 */
int dtb_verify_signature(void)
{
    const void *sig;
    int sig_len;
    int ret;
    
    if (!dtb_base) {
        return -EINVAL;
    }
    
    // Get signature property
    sig = fdt_getprop(dtb_base, 0, "signature", &sig_len);
    if (!sig) {
        pr_warn("DTB: No signature found\n");
        return 0;  // Not an error if signature optional
    }
    
    // Verify signature using public key
    ret = dtb_verify_rsa_signature(dtb_base, sig, sig_len);
    if (ret) {
        pr_err("DTB: Signature verification failed\n");
        return ret;
    }
    
    pr_info("DTB: Signature verified\n");
    
    return 0;
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("jk1806");
MODULE_DESCRIPTION("Device Tree Blob Implementation");
MODULE_VERSION(DTB_VERSION);

