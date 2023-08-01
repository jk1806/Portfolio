#include "boot.h"
#include "boot_config.h"
#include <stdint.h>
#include <string.h>

// v1.0 - Initial implementation
// Version counter for rollback protection

/* Simulated flash for version counter - TODO: Replace with actual flash driver */
static uint32_t version_flash[1] __attribute__((section(".version_section")));

int check_version_counter(uint32_t *version)
{
    if (!version) {
        return -1;
    }
    
    /* Read from flash */
    *version = version_flash[0];
    
    /* Initialize if zero or erased (0xFFFFFFFF) */
    if (*version == 0 || *version == 0xFFFFFFFF) {
        *version = MIN_VERSION;
        version_flash[0] = *version;
        // TODO: Should we write to actual flash here?
    }
    
    return 0;
}

int update_version_counter(uint32_t version)
{
    /* Write to flash */
    version_flash[0] = version;
    return 0;
}

