/**
 * GCC/Clang Compiler Integration
 * Author: jk1806
 * Created: 2024-02-05
 * 
 * Compiler wrapper and optimization settings
 * Supports both GCC and Clang toolchains
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define COMPILER_VERSION "1.0.0"

enum compiler_type {
    COMPILER_GCC,
    COMPILER_CLANG
};

static enum compiler_type current_compiler = COMPILER_GCC;

/**
 * Detect compiler
 */
enum compiler_type compiler_detect(void)
{
    FILE *fp;
    char output[256];
    
    // Try GCC first
    fp = popen("gcc --version 2>&1", "r");
    if (fp) {
        if (fgets(output, sizeof(output), fp)) {
            if (strstr(output, "gcc")) {
                pclose(fp);
                return COMPILER_GCC;
            }
        }
        pclose(fp);
    }
    
    // Try Clang
    fp = popen("clang --version 2>&1", "r");
    if (fp) {
        if (fgets(output, sizeof(output), fp)) {
            if (strstr(output, "clang")) {
                pclose(fp);
                return COMPILER_CLANG;
            }
        }
        pclose(fp);
    }
    
    return COMPILER_GCC;  // Default
}

/**
 * Get optimization flags
 */
const char *compiler_get_optimization_flags(int level)
{
    static const char *gcc_flags[] = {
        "-O0",  // No optimization
        "-O1",  // Basic optimization
        "-O2",  // Standard optimization
        "-O3",  // Aggressive optimization
        "-Os",  // Size optimization
    };
    
    static const char *clang_flags[] = {
        "-O0",
        "-O1",
        "-O2",
        "-O3",
        "-Oz",  // Clang size optimization
    };
    
    if (level < 0 || level > 4) {
        level = 2;  // Default
    }
    
    if (current_compiler == COMPILER_CLANG) {
        return clang_flags[level];
    }
    
    return gcc_flags[level];
}

/**
 * Compile source file
 */
int compiler_compile(const char *source, const char *output,
                     const char *flags, int opt_level)
{
    char cmd[1024];
    const char *compiler;
    const char *opt_flags;
    int ret;
    
    compiler = (current_compiler == COMPILER_CLANG) ? "clang" : "gcc";
    opt_flags = compiler_get_optimization_flags(opt_level);
    
    snprintf(cmd, sizeof(cmd),
             "%s %s %s -o %s %s",
             compiler, flags ? flags : "", opt_flags, output, source);
    
    printf("Compiling: %s\n", cmd);
    
    ret = system(cmd);
    
    return (ret == 0) ? 0 : -1;
}

