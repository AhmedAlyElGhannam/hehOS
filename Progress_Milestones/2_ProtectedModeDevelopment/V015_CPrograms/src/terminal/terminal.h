#ifndef TERMINAL_H
#define TERMINAL_H

#include <stddef.h>
#include <stdint.h>

#define VGA_WIDTH 80
#define VGA_HEIGHT 25

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

void terminal_initialize(void);
void terminal_clear(void);
void print(const char* str);
void print_with_colour(const char* str, char colour);
void terminal_writechar(char c, char colour);

void print_sussy_bakka(void); // SUS

#endif