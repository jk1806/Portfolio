#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

/* Fuzz target for embedded firmware parser */
int parse_firmware_header(const uint8_t *data, size_t size)
{
    if (size < 16) {
        return -1;
    }
    
    /* Parse header */
    uint32_t magic = *(uint32_t *)data;
    uint32_t version = *(uint32_t *)(data + 4);
    uint32_t size_field = *(uint32_t *)(data + 8);
    uint32_t checksum = *(uint32_t *)(data + 12);
    
    /* Validate magic */
    if (magic != 0x4649524D) { /* "FIRM" */
        return -1;
    }
    
    /* Validate version */
    if (version > 100) {
        return -1;
    }
    
    /* Validate size */
    if (size_field > size) {
        return -1;
    }
    
    /* Simple checksum validation */
    uint32_t calc_checksum = 0;
    for (size_t i = 0; i < size_field; i++) {
        calc_checksum += data[i];
    }
    
    if (calc_checksum != checksum) {
        return -1;
    }
    
    return 0;
}

/* AFL main function */
int main(int argc, char **argv)
{
    FILE *fp;
    uint8_t *data;
    size_t size;
    
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <input_file>\n", argv[0]);
        return 1;
    }
    
    fp = fopen(argv[1], "rb");
    if (!fp) {
        return 1;
    }
    
    fseek(fp, 0, SEEK_END);
    size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    
    data = malloc(size);
    if (!data) {
        fclose(fp);
        return 1;
    }
    
    fread(data, 1, size, fp);
    fclose(fp);
    
    parse_firmware_header(data, size);
    
    free(data);
    return 0;
}

