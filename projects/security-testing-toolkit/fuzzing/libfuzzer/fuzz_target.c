#include <stdint.h>
#include <stddef.h>

/* LibFuzzer target */
int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    if (size < 4) {
        return 0;
    }
    
    /* Parse input */
    uint32_t value = *(uint32_t *)data;
    
    /* Test for vulnerabilities */
    if (value == 0xDEADBEEF) {
        /* Potential vulnerability */
        volatile int *ptr = (int *)value;
        *ptr = 0;  /* Crash */
    }
    
    return 0;
}

