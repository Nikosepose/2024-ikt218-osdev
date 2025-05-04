#include "kernel/interrupt_functions.h"
#include "common/monitor.h"
#include "common/input.h"
#include "common/io.h"
#include "libc/stdbool.h"


// IRQ1 keyboard handler
void irq_keyboard_handler(registers_t* regs, void* context) {
    static unsigned char last_scancode = 0;
    static bool key_is_held = false;

    unsigned char scancode = inb(0x60);

    // Key release
    if (scancode & 0x80) {
        last_scancode = scancode & 0x7F;
        key_is_held = false;
        return;
    }

    if (scancode == last_scancode && key_is_held) {
        return; // Skip repeat if key still held
    }

    last_scancode = scancode;
    key_is_held = true;

    char c = scancode_to_ascii(&scancode);

    switch (c) {
        case 0:
            return;  // Nothing to do
        case 1:
            printf("[UP]\n");
            return;
        case 2:
            printf("[DOWN]\n");
            return;
        case 3:
            printf("[LEFT]\n");
            return;
        case 4:
            printf("[RIGHT]\n");
            return;
        case 5:
            monitor_backspace();
            return;
        case 6:
            monitor_enter();
            return;
        case 7:
            monitor_put_char(' ');
            return;
        default:
            monitor_put_char(c);
            return;
    }
}

// Registers your interrupt handlers
void init_interrupt_functions() {
    register_irq_handler(0, irq_keyboard_handler, NULL);       // IRQ 0 for keyboard
}