#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

#define DEVICE_TREE_PATH "/proc/device-tree"
#define MAX_DEVICE_TREE_SIZE (64 * 1024)

/* Device tree access control rules */
typedef struct {
    char path[256];
    uint32_t allowed_ops;  /* R=1, W=2, X=4 */
    uint32_t user_id;
} dt_access_rule_t;

static dt_access_rule_t access_rules[] = {
    {"/cpus", 1, 0},           /* Read-only for all */
    {"/memory", 1, 0},         /* Read-only for all */
    {"/soc/gpio", 3, 0},       /* Read-write for root */
    {"/soc/i2c", 1, 0},        /* Read-only */
    {"/soc/spi", 3, 1000},     /* Read-write for user 1000 */
};

#define NUM_RULES (sizeof(access_rules) / sizeof(access_rules[0]))

int check_device_tree_access(const char *dt_path, uint32_t operation, uint32_t user_id)
{
    int i;
    
    for (i = 0; i < NUM_RULES; i++) {
        if (strncmp(access_rules[i].path, dt_path, strlen(access_rules[i].path)) == 0) {
            /* Check user ID */
            if (access_rules[i].user_id != 0 && access_rules[i].user_id != user_id) {
                return -1; /* Access denied */
            }
            
            /* Check operation */
            if ((access_rules[i].allowed_ops & operation) == 0) {
                return -1; /* Operation not allowed */
            }
            
            return 0; /* Access allowed */
        }
    }
    
    /* Default: deny if no rule matches */
    return -1;
}

int read_device_tree_property(const char *dt_path, const char *property, 
                              void *buffer, size_t buffer_size)
{
    char full_path[512];
    int fd;
    ssize_t bytes;
    
    snprintf(full_path, sizeof(full_path), "%s%s/%s", DEVICE_TREE_PATH, dt_path, property);
    
    /* Check access */
    if (check_device_tree_access(dt_path, 1, getuid()) != 0) {
        return -1; /* Access denied */
    }
    
    fd = open(full_path, O_RDONLY);
    if (fd < 0) {
        return -1;
    }
    
    bytes = read(fd, buffer, buffer_size);
    close(fd);
    
    return bytes;
}

int write_device_tree_property(const char *dt_path, const char *property,
                              const void *data, size_t data_size)
{
    char full_path[512];
    int fd;
    ssize_t bytes;
    
    snprintf(full_path, sizeof(full_path), "%s%s/%s", DEVICE_TREE_PATH, dt_path, property);
    
    /* Check access */
    if (check_device_tree_access(dt_path, 2, getuid()) != 0) {
        return -1; /* Access denied */
    }
    
    fd = open(full_path, O_WRONLY);
    if (fd < 0) {
        return -1;
    }
    
    bytes = write(fd, data, data_size);
    close(fd);
    
    return bytes;
}

