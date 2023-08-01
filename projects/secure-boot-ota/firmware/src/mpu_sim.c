#include "firmware.h"
#include <stdint.h>
#include <stdio.h>

// v1.0 - Initial implementation
// MPU simulation for memory protection

/* Simulated MPU (Memory Protection Unit) - real hardware would use ARM MPU */
typedef struct {
    uint32_t base;
    uint32_t size;
    uint32_t permissions;
} mpu_region_t;

#define MPU_MAX_REGIONS 8  // ARM Cortex-M3/M4 typically has 8 regions
static mpu_region_t mpu_regions[MPU_MAX_REGIONS];
static int mpu_enabled = 0;

void mpu_init(void)
{
    /* Initialize MPU regions */
    /* Region 0: Code (read-only, execute) - Flash memory */
    mpu_regions[0].base = 0x08010000;  // STM32F4 flash start
    mpu_regions[0].size = 0x40000; /* 256 KB */
    mpu_regions[0].permissions = 0x05; /* R-X (read, execute) */
    
    /* Region 1: Data (read-write, no execute) - SRAM */
    mpu_regions[1].base = 0x20000000;  // STM32F4 SRAM start
    mpu_regions[1].size = 0x20000; /* 128 KB */
    mpu_regions[1].permissions = 0x06; /* RW- (read, write) */
    
    // TODO: Add more regions for peripherals, etc.
    
    mpu_enabled = 1;
    printf("[MPU] Memory protection enabled\n");
}

int mpu_check_access(uint32_t address, uint32_t access_type)
{
    int i;
    
    if (!mpu_enabled) {
        return 0; /* Allow if MPU disabled */
    }
    
    for (i = 0; i < MPU_MAX_REGIONS; i++) {
        if (address >= mpu_regions[i].base &&
            address < (mpu_regions[i].base + mpu_regions[i].size)) {
            /* Check permissions */
            if ((access_type & mpu_regions[i].permissions) != access_type) {
                return -1; /* Access denied */
            }
            return 0; /* Access allowed */
        }
    }
    
    return -1; /* No region found - deny access */
}

