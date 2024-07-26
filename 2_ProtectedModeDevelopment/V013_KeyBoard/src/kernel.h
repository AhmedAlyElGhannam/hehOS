#ifndef KERNEL_H
#define KERNEL_H

#include <stddef.h>
#include <stdint.h>

#define VGA_WIDTH 80
#define VGA_HEIGHT 20

#define VGA_COLOUR_BLACK        0
#define VGA_COLOUR_BLUE         1
#define VGA_COLOUR_GREEN        2
#define VGA_COLOUR_CYAN         3
#define VGA_COLOUR_RED          4
#define VGA_COLOUR_PURPLE       5
#define VGA_COLOUR_BROWN        6
#define VGA_COLOUR_GRAY         7
#define VGA_COLOUR_DARKGRAY     8
#define VGA_COLOUR_LIGHTBLUE    9
#define VGA_COLOUR_LIGHTGREEN   10
#define VGA_COLOUR_LIGHTCYAN    11
#define VGA_COLOUR_LIGHTRED     12
#define VGA_COLOUR_LIGHTPURPLE  13
#define VGA_COLOUR_YELLOW       14
#define VGA_COLOUR_WHITE        15

#define HEHOS_MAX_PATH 108

void kernel_main(void);
void print(const char* str);
void panic(const char* msg);
void kernel_page(void);

extern void kernel_registers(void);

#define ERROR(value)    (void*)(value)
#define ERROR_I(value)  (int)(value)
#define ISERR(value)    ((int)value < 0)

#endif