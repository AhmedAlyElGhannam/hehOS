#include "kernel.h"
#include <stddef.h>
#include <stdint.h>
#include "idt/idt.h"
#include "memory/heap/kheap.h"
#include "memory/paging/paging.h"
#include "status.h"
#include "string/string.h"
#include "fs/file.h"
#include "disk/disk.h"
#include "fs/pparser.h"
#include "disk/streamer.h"
#include "gdt/gdt.h"
#include "config.h"
#include "memory/memory.h"
#include "task/tss.h"
#include "task/task.h"
#include "task/process.h"
#include "status.h"
#include "isr80h/isr80h.h"
#include "keyboard/keyboard.h"

uint16_t* video_mem = 0;
uint16_t terminal_row = 0;
uint16_t terminal_col = 0;

void panic(const char* msg)
{
    print(msg);
    while(1);
}

uint16_t terminal_make_char(char c, char colour)
{
    return (colour << 8) | c;
}

void terminal_putchar(int x, int y, char c, char colour)
{
    video_mem[(y * VGA_WIDTH) + x] = terminal_make_char(c, colour);
}

void terminal_writechar(char c, char colour)
{
    if (c == '\n')
    {
        terminal_row += 1;
        terminal_col = 0;
        return;
    }

    if (c == '\t')
    {
        terminal_col += 3;
        if (terminal_col >= VGA_WIDTH)
        {
            terminal_col = 0;
            terminal_row += 1;
        }
        return;
    }
    
    terminal_putchar(terminal_col, terminal_row, c, colour);
    terminal_col += 1;

    if (terminal_col >= VGA_WIDTH)
    {
        terminal_col = 0;
        terminal_row += 1;
    }
}
void terminal_initialize(void)
{
    video_mem = (uint16_t*)(0xB8000);

    terminal_row = 0;
    terminal_col = 0;

    for (int y = 0; y < VGA_HEIGHT; y++)
    {
        for (int x = 0; x < VGA_WIDTH; x++)
        {
            terminal_putchar(x, y, ' ', 0);
        }
    }   
}

void print(const char* str)
{
    size_t len = strlen(str);

    for (size_t i = 0; i < len; i++)
    {
        terminal_writechar(str[i], VGA_COLOUR_WHITE);
    }
}

void print_with_colour(const char* str, char colour)
{
    size_t len = strlen(str);

    for (size_t i = 0; i < len; i++)
    {
        terminal_writechar(str[i], colour);
    }
}

void print_sussy_bakka(void)
{
    print("                    _______________");
    print("\n");
    print("                   /               \\");
    print("\n");
    print("                  /                 \\");
    print("\n");
    print("                 /     ");
    print_with_colour("_________", VGA_COLOUR_LIGHTBLUE);
    print("     \\");
    print("\n");
    print("                |");
    print_with_colour("     /         \\     ", VGA_COLOUR_LIGHTBLUE);
    print("\\");
    print("\n");
    print("               /     ");
    print_with_colour("(           )", VGA_COLOUR_LIGHTBLUE);
    print("    |");
    print("\n");
    print("               |      ");
    print_with_colour("\\_________/", VGA_COLOUR_LIGHTBLUE);
    print("     |");
    print("\n");
    print("              /                       \\");
    print("\n");
    print("              |                        |");
    print("\n");
    print("              |                        |");
    print("\n");
    print("             /                         |");
    print("\n");
    print("             |                         |");
    print("\n");
    print("             |                         |");
    print("\n");
    print("            /                          |");
    print("\n");
    print("            |                          |");
    print("\n");
    print("            |        ________          |");
    print("\n");
    print("      _____/        /        \\         |");
    print("\n");
    print("     /              |        |         |");
    print("\n");
    print("     |             /   _____/          |");
    print("\n");
    print("     \\____________/   /               _|");
    print("\n");
    print("                      \\            __/");
    print("\n");
    print("                       \\__________/");
    print("\n");
}

extern void problemo(void);

static struct paging_4gb_chunk* kernel_chunk = 0;

void kernel_page(void)
{
    kernel_registers();
    paging_switch(kernel_chunk);
}

struct tss tss;

struct gdt gdt_real[HEHOS_TOTAL_GDT_SEGMENTS];
struct gdt_structured gdt_structured[HEHOS_TOTAL_GDT_SEGMENTS] = {
    {.base = 0x00, .limit = 0x00, .type = 0x00},                       // null segment
    {.base = 0x00, .limit = 0xFFFFFFFF, .type = 0x9A},                 // kernel code segment
    {.base = 0x00, .limit = 0xFFFFFFFF, .type = 0x92},                 // kernel data segment 
    {.base = 0x00, .limit = 0xFFFFFFFF, .type = 0xF8},                 // user code segment 
    {.base = 0x00, .limit = 0xFFFFFFFF, .type = 0xF2},                 // user data segment
    {.base = (uint32_t)&tss, .limit = sizeof(tss), .type = 0xE9}       // tss segment 
};

void kernel_main(void)
{
    // clear terminal
    terminal_initialize();

    memset(gdt_real, 0x00, sizeof(gdt_real));
    gdt_structured_to_gdt(gdt_real, gdt_structured, HEHOS_TOTAL_GDT_SEGMENTS);
    // load gdt
    gdt_load(gdt_real, sizeof(gdt_real));

    //print_sussy_bakka();

    // initialize the heap
    kheap_init();

    // initialize filesystems
    fs_init();

    // search and initialize the disks
    disk_search_and_init();

    // initialize the interrupt descriptor table
    idt_init();

    // setting up Task Switching Segment
    memset(&tss, 0x00, sizeof(tss));
    tss.esp0 = 0x600000; // where kernel stack is located
    tss.ss0 = KERNEL_DATA_SELECTOR;

    // load Task Switching Segment
    tss_load(0x28); // offset in gdt after conversion

    // paging setup
    kernel_chunk = paging_new_4gb(PAGING_IS_WRITEABLE | PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL);
    
    // switch to kernel paging chunk
    paging_switch(kernel_chunk);

    // enable paging
    enable_paging();
    
    // register kernel commands
    isr80_register_commands();

    // initialize all of system's keyboards 
    keyboard_init();

    // enable interrupts
    // enable_interrupts();

    struct process* process = 0;
    int res = process_load("0:/blank.bin", &process);
    if (res != HEHOS_ALL_OK)
    {
        panic("Failed to Load blank.bin!\n");
    }

    print_sussy_bakka();
    task_run_first_ever_task();
    
    while(1);
}