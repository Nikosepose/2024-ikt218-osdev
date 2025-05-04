//kernel.c
#include "libc/stdint.h"
#include "libc/stddef.h"
#include "libc/stdbool.h"
#include "libc/string.h"
#include "libc/stdio.h"

#include "kernel/gdt.h"
#include "kernel/interrupts.h"

#include "common/monitor.h"  
#include "common/io.h"  

#include <multiboot2.h>



struct multiboot_info {
    uint32_t size;
    uint32_t reserved;
    struct multiboot_tag *first;
};

int kernel_main();

void wait_for_keypress() {
    asm volatile("sti"); // Ensure interrupts are enabled

    volatile bool key_pressed = false;

    void temp_key_callback(registers_t* regs, void* context) {
        key_pressed = true;
    }

    register_irq_handler(1, temp_key_callback, NULL);

    while (!key_pressed) {
        asm volatile("hlt"); // Halt CPU until next interrupt
    }
    unregister_irq_handler(1);
}

int kernel_main_c(uint32_t magic, struct multiboot_info* mb_info_addr) {
    // Initialize the monitor
    monitor_init(); 
    // Initialize GDT
    printf("Initializing GDT...\n");
    init_gdt(); 
    // Initialize IDT
    printf("Initializing IDT...\n");
    init_idt();
    //initialize interrupts
    printf("Initializing interrupts...\n");
    init_irq();
    printf("All kernel functionality is good to \n");
    wait_for_keypress(); // Wait for a keypress before proceeding
    monitor_clear(); // Clear the monitor screen
    // Call cpp kernel_main (defined in kernel.cpp)
    return kernel_main();
}