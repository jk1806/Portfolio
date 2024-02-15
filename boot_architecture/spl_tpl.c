/**
 * SPL/TPL (Secondary/Tertiary Program Loader) Implementation
 * Author: jk1806
 * Created: 2024-01-15
 * 
 * Boot ROM secondary loader for ARM Cortex-A platforms
 * Handles initial DRAM initialization and primary bootloader loading
 */

#include <common.h>
#include <spl.h>
#include <image.h>
#include <asm/io.h>
#include <asm/arch/spl.h>

#define SPL_VERSION "1.2.0"
#define SPL_MAX_SIZE (64 * 1024)  // 64KB limit
#define TPL_MAX_SIZE (32 * 1024)  // 32KB limit

struct spl_image_info {
    u32 os;
    u32 load_addr;
    u32 entry_point;
    u32 size;
    u32 flags;
    const char *name;
};

static struct spl_image_info spl_image;

/**
 * Initialize DRAM controller
 * This runs before main SPL to set up memory
 */
static int spl_dram_init(void)
{
    struct dram_controller *dram;
    u32 reg;
    
    dram = (struct dram_controller *)DRAM_BASE;
    
    // Configure DRAM timing parameters
    writel(0x12345678, &dram->timing_ctrl);
    writel(0x9abcdef0, &dram->timing_ctrl2);
    
    // Enable DRAM controller
    reg = readl(&dram->ctrl);
    reg |= DRAM_CTRL_ENABLE;
    writel(reg, &dram->ctrl);
    
    // Wait for DRAM ready
    int timeout = 1000;
    while (timeout-- > 0) {
        if (readl(&dram->status) & DRAM_STATUS_READY)
            break;
        udelay(100);
    }
    
    if (timeout <= 0) {
        printf("SPL: DRAM init timeout\n");
        return -1;
    }
    
    printf("SPL: DRAM initialized at 0x%x\n", DRAM_BASE);
    return 0;
}

/**
 * Load SPL image from storage
 */
static int spl_load_image(enum boot_device device)
{
    int ret;
    struct image_header *header;
    u32 load_addr = CONFIG_SYS_TEXT_BASE;
    
    printf("SPL: Loading from device %d\n", device);
    
    switch (device) {
    case BOOT_DEVICE_MMC1:
        ret = spl_mmc_load_image(CONFIG_SYS_MMCSD_RAW_MODE_U_BOOT_SECTOR,
                                 load_addr);
        break;
    case BOOT_DEVICE_NAND:
        ret = spl_nand_load_image(load_addr);
        break;
    case BOOT_DEVICE_SPI:
        ret = spl_spi_load_image(load_addr);
        break;
    default:
        printf("SPL: Unsupported boot device\n");
        return -1;
    }
    
    if (ret) {
        printf("SPL: Failed to load image\n");
        return ret;
    }
    
    header = (struct image_header *)load_addr;
    
    // Verify image header
    if (image_get_magic(header) != IH_MAGIC) {
        printf("SPL: Invalid image magic\n");
        return -1;
    }
    
    spl_image.os = image_get_os(header);
    spl_image.load_addr = image_get_load(header);
    spl_image.entry_point = image_get_ep(header);
    spl_image.size = image_get_data_size(header);
    spl_image.name = image_get_name(header);
    
    printf("SPL: Image loaded: %s, size: %d bytes\n", 
           spl_image.name, spl_image.size);
    
    return 0;
}

/**
 * Jump to next boot stage
 */
void spl_jump_to_next_stage(void)
{
    typedef void __noreturn (*image_entry_noargs_t)(void);
    image_entry_noargs_t entry;
    
    printf("SPL: Jumping to entry point 0x%x\n", spl_image.entry_point);
    
    // Flush caches
    flush_cache(spl_image.load_addr, spl_image.size);
    
    // Disable interrupts
    disable_interrupts();
    
    // Jump to entry point
    entry = (image_entry_noargs_t)spl_image.entry_point;
    entry();
}

/**
 * SPL main entry point
 */
void board_init_f(ulong dummy)
{
    int ret;
    enum boot_device boot_dev;
    
    printf("\nSPL: Secondary Program Loader v%s\n", SPL_VERSION);
    
    // Initialize board
    board_early_init_f();
    
    // Initialize DRAM
    ret = spl_dram_init();
    if (ret) {
        hang();
    }
    
    // Relocate to DRAM
    spl_relocate_stack_gd();
    
    // Detect boot device
    boot_dev = spl_boot_device();
    printf("SPL: Boot device: %d\n", boot_dev);
    
    // Load next stage
    ret = spl_load_image(boot_dev);
    if (ret) {
        printf("SPL: Failed to load image, hanging\n");
        hang();
    }
    
    // Jump to next stage (U-Boot or kernel)
    spl_jump_to_next_stage();
}

