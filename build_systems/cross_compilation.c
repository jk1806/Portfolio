/**
 * Cross-Compilation Support
 * Author: jk1806
 * Created: 2024-02-12
 * 
 * Cross-compilation toolchain management
 * Supports ARM, RISC-V, MIPS architectures
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CROSS_COMPILE_VERSION "1.0.0"

struct cross_toolchain {
    char prefix[32];
    char arch[16];
    char *gcc_path;
    char *ld_path;
    char *objcopy_path;
};

static struct cross_toolchain toolchains[] = {
    {"arm-linux-gnueabihf-", "arm", NULL, NULL, NULL},
    {"aarch64-linux-gnu-", "aarch64", NULL, NULL, NULL},
    {"riscv64-linux-gnu-", "riscv64", NULL, NULL, NULL},
    {"mips-linux-gnu-", "mips", NULL, NULL, NULL},
};

/**
 * Find cross-compiler
 */
int cross_compile_find_toolchain(const char *arch)
{
    int i;
    char cmd[256];
    FILE *fp;
    
    for (i = 0; i < sizeof(toolchains) / sizeof(toolchains[0]); i++) {
        if (strcmp(toolchains[i].arch, arch) == 0) {
            // Check if toolchain exists
            snprintf(cmd, sizeof(cmd), "which %sgcc", toolchains[i].prefix);
            fp = popen(cmd, "r");
            if (fp) {
                if (fgets(cmd, sizeof(cmd), fp)) {
                    pclose(fp);
                    toolchains[i].gcc_path = strdup(cmd);
                    strtok(toolchains[i].gcc_path, "\n");
                    return i;
                }
                pclose(fp);
            }
        }
    }
    
    return -1;
}

/**
 * Cross-compile source
 */
int cross_compile_build(const char *arch, const char *source,
                        const char *output, const char *flags)
{
    int toolchain_idx;
    char cmd[1024];
    
    toolchain_idx = cross_compile_find_toolchain(arch);
    if (toolchain_idx < 0) {
        printf("Cross-compile: Toolchain not found for %s\n", arch);
        return -1;
    }
    
    snprintf(cmd, sizeof(cmd),
             "%sgcc %s -o %s %s",
             toolchains[toolchain_idx].prefix,
             flags ? flags : "",
             output, source);
    
    printf("Cross-compiling for %s: %s\n", arch, cmd);
    
    return system(cmd);
}

