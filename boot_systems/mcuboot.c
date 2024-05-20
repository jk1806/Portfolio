/**
 * MCUboot Bootloader Implementation
 * Author: jk1806
 * Created: 2024-04-05
 * 
 * Secure bootloader for microcontroller units
 * Supports A/B image slots and image validation
 */

#include <mcuboot_config/mcuboot_config.h>
#include <bootutil/bootutil.h>
#include <bootutil/image.h>
#include <bootutil/sign_key.h>
#include <flash_map_backend/flash_map.h>

#define MCUBOOT_VERSION "1.8.0"
#define IMAGE_SLOT_A 0
#define IMAGE_SLOT_B 1

struct boot_swap_state {
    uint8_t magic;
    uint8_t swap_type;
    uint8_t image_num;
    uint8_t copy_done;
    uint8_t image_ok;
};

static struct boot_swap_state swap_state;

/**
 * Read swap state from flash
 */
static int boot_read_swap_state(int slot, struct boot_swap_state *state)
{
    const struct flash_area *fap;
    int rc;
    
    rc = flash_area_open(FLASH_AREA_IMAGE_SCRATCH(slot), &fap);
    if (rc) {
        return rc;
    }
    
    rc = flash_area_read(fap, 0, state, sizeof(*state));
    flash_area_close(fap);
    
    return rc;
}

/**
 * Determine which image slot to boot
 */
static int boot_swap_type(void)
{
    struct boot_swap_state state_a, state_b;
    int rc;
    
    // Read swap state from both slots
    rc = boot_read_swap_state(IMAGE_SLOT_A, &state_a);
    if (rc) {
        return BOOT_SWAP_TYPE_NONE;
    }
    
    rc = boot_read_swap_state(IMAGE_SLOT_B, &state_b);
    if (rc) {
        return BOOT_SWAP_TYPE_NONE;
    }
    
    // Check for permanent swap
    if (state_a.magic == BOOT_MAGIC_GOOD &&
        state_a.swap_type == BOOT_SWAP_TYPE_PERM) {
        return BOOT_SWAP_TYPE_PERM;
    }
    
    // Check for revert swap
    if (state_a.magic == BOOT_MAGIC_GOOD &&
        state_a.swap_type == BOOT_SWAP_TYPE_REVERT) {
        return BOOT_SWAP_TYPE_REVERT;
    }
    
    // Check slot B for new image
    if (state_b.magic == BOOT_MAGIC_GOOD &&
        state_b.copy_done == 0x01) {
        return BOOT_SWAP_TYPE_TEST;
    }
    
    return BOOT_SWAP_TYPE_NONE;
}

/**
 * Validate image in slot
 */
static int boot_validate_slot(int slot, struct image_header *hdr)
{
    const struct flash_area *fap;
    uint8_t hash[32];
    int rc;
    
    rc = flash_area_open(FLASH_AREA_IMAGE_SLOT(slot), &fap);
    if (rc) {
        return rc;
    }
    
    // Read image header
    rc = flash_area_read(fap, 0, hdr, sizeof(*hdr));
    if (rc) {
        flash_area_close(fap);
        return rc;
    }
    
    // Verify image magic
    if (hdr->ih_magic != IMAGE_MAGIC) {
        flash_area_close(fap);
        return -1;
    }
    
    // Verify image version
    if (hdr->ih_ver.major != IMAGE_VERSION_MAJOR) {
        flash_area_close(fap);
        return -1;
    }
    
    // Verify signature if enabled
    #ifdef MCUBOOT_SIGN_RSA
    rc = bootutil_img_validate(MCUBOOT_SIGN_RSA, fap, hdr, hash, 0);
    if (rc) {
        flash_area_close(fap);
        return rc;
    }
    #endif
    
    flash_area_close(fap);
    
    return 0;
}

/**
 * Copy image from slot B to slot A
 */
static int boot_copy_image(int src_slot, int dst_slot)
{
    const struct flash_area *src_fap, *dst_fap;
    uint8_t buf[256];
    size_t off;
    int rc;
    
    rc = flash_area_open(FLASH_AREA_IMAGE_SLOT(src_slot), &src_fap);
    if (rc) {
        return rc;
    }
    
    rc = flash_area_open(FLASH_AREA_IMAGE_SLOT(dst_slot), &dst_fap);
    if (rc) {
        flash_area_close(src_fap);
        return rc;
    }
    
    // Erase destination slot
    rc = flash_area_erase(dst_fap, 0, dst_fap->fa_size);
    if (rc) {
        flash_area_close(src_fap);
        flash_area_close(dst_fap);
        return rc;
    }
    
    // Copy image data
    for (off = 0; off < src_fap->fa_size; off += sizeof(buf)) {
        size_t read_size = (src_fap->fa_size - off > sizeof(buf)) ?
                          sizeof(buf) : (src_fap->fa_size - off);
        
        rc = flash_area_read(src_fap, off, buf, read_size);
        if (rc) {
            break;
        }
        
        rc = flash_area_write(dst_fap, off, buf, read_size);
        if (rc) {
            break;
        }
    }
    
    flash_area_close(src_fap);
    flash_area_close(dst_fap);
    
    return rc;
}

/**
 * MCUboot main entry point
 */
int main(void)
{
    struct image_header hdr;
    int swap_type;
    int boot_slot;
    int rc;
    
    printf("MCUboot v%s starting\n", MCUBOOT_VERSION);
    
    // Determine swap type
    swap_type = boot_swap_type();
    printf("MCUboot: Swap type: %d\n", swap_type);
    
    // Select boot slot
    if (swap_type == BOOT_SWAP_TYPE_TEST ||
        swap_type == BOOT_SWAP_TYPE_PERM) {
        boot_slot = IMAGE_SLOT_B;
    } else {
        boot_slot = IMAGE_SLOT_A;
    }
    
    // Validate image in selected slot
    rc = boot_validate_slot(boot_slot, &hdr);
    if (rc) {
        printf("MCUboot: Image validation failed, trying other slot\n");
        boot_slot = (boot_slot == IMAGE_SLOT_A) ? IMAGE_SLOT_B : IMAGE_SLOT_A;
        rc = boot_validate_slot(boot_slot, &hdr);
        if (rc) {
            printf("MCUboot: No valid image found\n");
            return -1;
        }
    }
    
    printf("MCUboot: Booting from slot %d\n", boot_slot);
    
    // If permanent swap, copy image
    if (swap_type == BOOT_SWAP_TYPE_PERM) {
        int dst_slot = (boot_slot == IMAGE_SLOT_A) ? IMAGE_SLOT_A : IMAGE_SLOT_B;
        rc = boot_copy_image(boot_slot, dst_slot);
        if (rc) {
            printf("MCUboot: Failed to copy image\n");
            return rc;
        }
    }
    
    // Jump to application
    boot_jump_to_image(&hdr, boot_slot);
    
    return 0;
}

