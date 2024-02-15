/**
 * FIT/ITB (Flattened Image Tree) Implementation
 * Author: jk1806
 * Created: 2024-03-10
 * 
 * U-Boot FIT image format parser
 * Handles device tree blob with multiple kernel/ramdisk images
 */

#include <common.h>
#include <image.h>
#include <fit.h>
#include <fdt_support.h>
#include <linux/libfdt.h>

#define FIT_VERSION "1.1.0"
#define FIT_MAX_IMAGES 16

struct fit_image_info {
    const char *name;
    void *data;
    size_t size;
    ulong load_addr;
    ulong entry_point;
    uint8_t type;
    uint8_t comp;
    int arch;
    int os;
};

static struct fit_image_info fit_images[FIT_MAX_IMAGES];
static int fit_image_count = 0;

/**
 * Get FIT property
 */
static const void *fit_get_prop(const void *fit, int noffset,
                                 const char *propname, int *lenp)
{
    return fdt_getprop(fit, noffset, propname, lenp);
}

/**
 * Get image node from FIT
 */
static int fit_get_image_node(const void *fit, const char *image_name)
{
    int images_noffset;
    int noffset;
    const char *name;
    
    images_noffset = fdt_path_offset(fit, FIT_IMAGES_PATH);
    if (images_noffset < 0) {
        printf("FIT: Cannot find images node\n");
        return -1;
    }
    
    for (noffset = fdt_first_subnode(fit, images_noffset);
         noffset >= 0;
         noffset = fdt_next_subnode(fit, noffset)) {
        
        name = fit_get_name(fit, noffset, NULL);
        if (name && strcmp(name, image_name) == 0)
            return noffset;
    }
    
    return -1;
}

/**
 * Load image data from FIT
 */
static int fit_load_image_data(const void *fit, int noffset,
                                struct fit_image_info *info)
{
    const void *data;
    int len;
    ulong load_addr;
    
    // Get load address
    data = fit_get_prop(fit, noffset, FIT_LOAD_PROP, &len);
    if (data) {
        load_addr = fit_read_number(data, len);
        info->load_addr = load_addr;
    } else {
        info->load_addr = CONFIG_SYS_LOAD_ADDR;
    }
    
    // Get entry point
    data = fit_get_prop(fit, noffset, FIT_ENTRY_PROP, &len);
    if (data) {
        info->entry_point = fit_read_number(data, len);
    } else {
        info->entry_point = info->load_addr;
    }
    
    // Get image type
    data = fit_get_prop(fit, noffset, FIT_TYPE_PROP, &len);
    if (data) {
        info->type = image_get_type((char *)data);
    }
    
    // Get compression
    data = fit_get_prop(fit, noffset, FIT_COMP_PROP, &len);
    if (data) {
        info->comp = image_get_comp((char *)data);
    }
    
    // Get architecture
    data = fit_get_prop(fit, noffset, FIT_ARCH_PROP, &len);
    if (data) {
        info->arch = genimg_get_arch_id((char *)data);
    }
    
    // Get OS
    data = fit_get_prop(fit, noffset, FIT_OS_PROP, &len);
    if (data) {
        info->os = genimg_get_os_id((char *)data);
    }
    
    // Load actual image data
    data = fit_get_prop(fit, noffset, FIT_DATA_PROP, &len);
    if (!data) {
        printf("FIT: No data property in image\n");
        return -1;
    }
    
    info->data = (void *)data;
    info->size = len;
    
    printf("FIT: Loaded image: addr=0x%lx, size=%zu, type=%d\n",
           info->load_addr, info->size, info->type);
    
    return 0;
}

/**
 * Parse FIT image
 */
int fit_image_load(ulong addr, const char *image_name,
                   struct fit_image_info *info)
{
    const void *fit;
    int noffset;
    int ret;
    
    printf("FIT: Loading image '%s' from 0x%lx\n", image_name, addr);
    
    // Check FIT magic
    fit = (const void *)addr;
    if (fdt_check_header(fit)) {
        printf("FIT: Invalid device tree header\n");
        return -1;
    }
    
    // Verify FIT structure
    if (!fit_check_format(fit)) {
        printf("FIT: Invalid FIT format\n");
        return -1;
    }
    
    // Find image node
    noffset = fit_get_image_node(fit, image_name);
    if (noffset < 0) {
        printf("FIT: Image '%s' not found\n", image_name);
        return -1;
    }
    
    // Load image data
    ret = fit_load_image_data(fit, noffset, info);
    if (ret) {
        printf("FIT: Failed to load image data\n");
        return ret;
    }
    
    // Verify hash if present
    if (fit_image_verify(fit, noffset) < 0) {
        printf("FIT: Image verification failed\n");
        return -1;
    }
    
    printf("FIT: Image '%s' loaded successfully\n", image_name);
    
    return 0;
}

/**
 * Get default configuration from FIT
 */
const char *fit_get_default_config(const void *fit)
{
    int noffset;
    const char *config;
    int len;
    
    noffset = fdt_path_offset(fit, FIT_CONFS_PATH);
    if (noffset < 0)
        return NULL;
    
    config = fit_get_prop(fit, noffset, FIT_DEFAULT_PROP, &len);
    if (!config)
        return NULL;
    
    return config;
}

