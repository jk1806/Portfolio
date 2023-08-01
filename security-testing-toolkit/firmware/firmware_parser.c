#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

typedef struct {
    uint32_t magic;
    uint32_t version;
    uint32_t size;
    uint32_t checksum;
} firmware_header_t;

#define FIRMWARE_MAGIC 0x4649524D  /* "FIRM" */

int parse_firmware(const char *filename)
{
    FILE *fp;
    firmware_header_t header;
    uint8_t *data;
    uint32_t calculated_checksum = 0;
    int i;
    
    fp = fopen(filename, "rb");
    if (!fp) {
        return -1;
    }
    
    /* Read header */
    fread(&header, sizeof(header), 1, fp);
    
    /* Verify magic */
    if (header.magic != FIRMWARE_MAGIC) {
        printf("Invalid firmware magic: 0x%08x\n", header.magic);
        fclose(fp);
        return -1;
    }
    
    printf("Firmware Header:\n");
    printf("  Magic: 0x%08x\n", header.magic);
    printf("  Version: %u\n", header.version);
    printf("  Size: %u bytes\n", header.size);
    printf("  Checksum: 0x%08x\n", header.checksum);
    
    /* Read firmware data */
    data = malloc(header.size);
    fread(data, 1, header.size, fp);
    
    /* Calculate checksum */
    for (i = 0; i < header.size; i++) {
        calculated_checksum += data[i];
    }
    
    printf("  Calculated checksum: 0x%08x\n", calculated_checksum);
    
    if (calculated_checksum != header.checksum) {
        printf("  [WARNING] Checksum mismatch!\n");
    } else {
        printf("  [OK] Checksum verified\n");
    }
    
    free(data);
    fclose(fp);
    return 0;
}

int main(int argc, char **argv)
{
    if (argc < 2) {
        printf("Usage: %s <firmware_file>\n", argv[0]);
        return 1;
    }
    
    return parse_firmware(argv[1]);
}

