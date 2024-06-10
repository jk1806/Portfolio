/**
 * UART ROM Boot Implementation
 * Author: jk1806
 * Created: 2024-06-05
 * 
 * UART-based ROM bootloader for recovery and factory programming
 * Supports XMODEM, YMODEM protocols for image transfer
 */

#include <common.h>
#include <serial.h>
#include <xmodem.h>

#define UART_ROM_BOOT_VERSION "1.0.0"
#define UART_BAUDRATE 115200
#define BOOT_DELAY_MS 3000

/**
 * Wait for boot command from UART
 */
static int uart_rom_wait_for_command(void)
{
    char cmd;
    ulong start_time;
    int timeout = BOOT_DELAY_MS;
    
    printf("\nUART ROM Boot v%s\n", UART_ROM_BOOT_VERSION);
    printf("Waiting for boot command...\n");
    printf("Press 's' for serial download, 'b' for normal boot\n");
    
    start_time = get_timer(0);
    
    while (get_timer(start_time) < timeout) {
        if (serial_tstc()) {
            cmd = serial_getc();
            
            if (cmd == 's' || cmd == 'S') {
                printf("Serial download mode selected\n");
                return 1;
            } else if (cmd == 'b' || cmd == 'B') {
                printf("Normal boot selected\n");
                return 0;
            }
        }
        
        udelay(10000);  // 10ms delay
    }
    
    printf("Timeout, proceeding with normal boot\n");
    return 0;
}

/**
 * Load image via XMODEM
 */
static int uart_rom_load_xmodem(ulong load_addr)
{
    int ret;
    struct xmodem_ctx ctx;
    
    printf("Waiting for XMODEM transfer...\n");
    printf("Load address: 0x%lx\n", load_addr);
    
    xmodem_init(&ctx, load_addr);
    
    ret = xmodem_receive(&ctx);
    if (ret) {
        printf("XMODEM transfer failed: %d\n", ret);
        return ret;
    }
    
    printf("XMODEM transfer complete: %zu bytes\n", ctx.total_bytes);
    
    return 0;
}

/**
 * Load image via YMODEM
 */
static int uart_rom_load_ymodem(ulong load_addr)
{
    int ret;
    char filename[256];
    size_t file_size;
    
    printf("Waiting for YMODEM transfer...\n");
    
    ret = ymodem_receive(&load_addr, filename, sizeof(filename), &file_size);
    if (ret) {
        printf("YMODEM transfer failed: %d\n", ret);
        return ret;
    }
    
    printf("YMODEM transfer complete: %s, %zu bytes\n", filename, file_size);
    
    return 0;
}

/**
 * Verify loaded image
 */
static int uart_rom_verify_image(ulong load_addr, size_t size)
{
    struct image_header *hdr;
    
    hdr = (struct image_header *)load_addr;
    
    // Check image magic
    if (image_get_magic(hdr) != IH_MAGIC) {
        printf("Invalid image magic\n");
        return -1;
    }
    
    // Check image type
    if (image_get_type(hdr) != IH_TYPE_KERNEL &&
        image_get_type(hdr) != IH_TYPE_FIRMWARE) {
        printf("Unsupported image type\n");
        return -1;
    }
    
    printf("Image verified: %s\n", image_get_name(hdr));
    
    return 0;
}

/**
 * Jump to loaded image
 */
static void uart_rom_jump_to_image(ulong entry_point)
{
    typedef void __noreturn (*image_entry_t)(void);
    image_entry_t entry;
    
    printf("Jumping to entry point: 0x%lx\n", entry_point);
    
    // Flush caches
    flush_cache(entry_point, 0);
    
    // Disable interrupts
    disable_interrupts();
    
    // Jump
    entry = (image_entry_t)entry_point;
    entry();
}

/**
 * UART ROM boot main
 */
void uart_rom_boot(void)
{
    int cmd;
    ulong load_addr = CONFIG_SYS_LOAD_ADDR;
    int ret;
    
    // Initialize UART
    serial_init();
    serial_setbrg(UART_BAUDRATE);
    
    // Wait for command
    cmd = uart_rom_wait_for_command();
    
    if (cmd == 0) {
        // Normal boot - return to caller
        return;
    }
    
    // Serial download mode
    printf("\n=== UART ROM Boot Menu ===\n");
    printf("1. XMODEM download\n");
    printf("2. YMODEM download\n");
    printf("Select protocol (1/2): ");
    
    cmd = serial_getc();
    serial_putc(cmd);
    printf("\n");
    
    if (cmd == '1') {
        ret = uart_rom_load_xmodem(load_addr);
    } else if (cmd == '2') {
        ret = uart_rom_load_ymodem(load_addr);
    } else {
        printf("Invalid selection\n");
        return;
    }
    
    if (ret) {
        printf("Download failed\n");
        return;
    }
    
    // Verify image
    ret = uart_rom_verify_image(load_addr, 0);
    if (ret) {
        printf("Image verification failed\n");
        return;
    }
    
    // Get entry point
    struct image_header *hdr = (struct image_header *)load_addr;
    ulong entry_point = image_get_ep(hdr);
    
    // Jump to image
    uart_rom_jump_to_image(entry_point);
}

