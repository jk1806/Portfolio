#include "boot.h"
#include <stdint.h>

// v1.0 - Initial implementation
// Jump to firmware - critical function, must be correct

/* Jump to firmware entry point */
void jump_to_firmware(void *address)
{
    /* Function pointer to firmware entry */
    void (*firmware_entry)(void) = (void (*)(void))address;
    
    /* Disable interrupts before jump - ARM Cortex-M instruction */
    __asm__ volatile ("cpsid i");
    
    /* Set stack pointer if needed - commented out for now */
    /* TODO: Get stack pointer from firmware header */
    /* __asm__ volatile ("mov sp, %0" : : "r" (stack_ptr)); */
    
    /* Jump to firmware - point of no return */
    firmware_entry();
    
    /* Should not return */
    while (1) {
        /* Halt */
    }
}

