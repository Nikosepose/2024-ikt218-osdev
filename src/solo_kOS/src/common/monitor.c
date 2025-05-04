// monitor.c
#include "common/monitor.h"

// First we define the size of the VGA text mode screen
#define VGA_WIDTH 80
#define VGA_HEIGHT 25

// This is the global monitor instance, kept static so it’s private to this file
static monitor_t monitor;

/*
Initializes the monitor state:
    - Sets cursor to top-left (row 0, col 0)
    - Points video_memory to the start of VGA memory
    - Sets the default color to light grey on black (0x07)
*/
void monitor_init() {
    monitor.cursor_row = 0;
    monitor.cursor_col = 0;
    monitor.video_memory = (volatile char*)0xb8000;
    monitor.color = 0x07; // Light grey on black
}

// This is where we output a character to the monitor
void monitor_put_char(char c) {
    // If the character is a newline, move to the beginning of the next line
    if (c == '\n') {
        monitor.cursor_col = 0;
        monitor.cursor_row++;
    } else {
        // Each character on screen takes up 2 bytes: 1 for the char, 1 for its color
        size_t index = (monitor.cursor_row * VGA_WIDTH + monitor.cursor_col) * 2;

        // Write the character and its color attribute to video memory
        monitor.video_memory[index] = c;
        monitor.video_memory[index + 1] = monitor.color;

        monitor.cursor_col++;

        // If we reach the end of the line, move to the next row
        if (monitor.cursor_col >= VGA_WIDTH) {
            monitor.cursor_col = 0;
            monitor.cursor_row++;
        }
    }

    // If we reach the bottom of the screen, reset to top (no scrolling yet)
    if (monitor.cursor_row >= VGA_HEIGHT) {
        monitor.cursor_row = 0; // TODO: implement scrolling
    }
}

void monitor_backspace() {
    // Don't go past the top-left corner
    if (monitor.cursor_col == 0 && monitor.cursor_row == 0) {
        return;
    }

    // Move the cursor back one position
    if (monitor.cursor_col == 0) {
        monitor.cursor_row--;
        monitor.cursor_col = VGA_WIDTH - 1;
    } else {
        monitor.cursor_col--;
    }

    // Calculate the index and clear the character
    size_t index = (monitor.cursor_row * VGA_WIDTH + monitor.cursor_col) * 2;
    monitor.video_memory[index] = ' ';
    monitor.video_memory[index + 1] = monitor.color;
}

void monitor_enter() {
    monitor.cursor_col = 0;
    monitor.cursor_row++;

    // Prevent cursor from going off screen
    if (monitor.cursor_row >= VGA_HEIGHT) {
        monitor.cursor_row = 0; // Or scroll later
    }
}

// Writes a null-terminated string to the screen, one character at a time
void monitor_write(const char* str) {
    for (size_t i = 0; str[i] != '\0'; i++) {
        monitor_put_char(str[i]);
    }
}

// Clears the screen by filling it with spaces and resetting the cursor
void monitor_clear() {
    for (int row = 0; row < VGA_HEIGHT; row++) {
        for (int col = 0; col < VGA_WIDTH; col++) {
            size_t index = (row * VGA_WIDTH + col) * 2;
            monitor.video_memory[index] = ' ';
            monitor.video_memory[index + 1] = monitor.color;
        }
    }

    monitor.cursor_row = 0;
    monitor.cursor_col = 0;
}


void monitor_write_dec(int num) {
    char buffer[16];
    int i = 0;

    if (num == 0) {
        monitor_put_char('0');
        return;
    }

    while (num > 0) {
        buffer[i++] = '0' + (num % 10);
        num /= 10;
    }

    for (int j = i - 1; j >= 0; j--) {
        monitor_put_char(buffer[j]);
    }
}