#include "kernel.h"
#include "idt/idt.h"
#include "io/io.h"
#include "memory/heap/kheap.h"
#include "memory/paging/paging.h"
#include "disk/disk.h"
#include "string/string.h"
#include "fs/pparser.h"
#include "disk/dstreamer.h"


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

// make kernel paging only accessible from here
static struct paging_4gb_chunk* kernel_chunk = 0;

void kernel_main(void)
{
    terminal_initialize();

    print_sussy_bakka();

    // initialize heap
    kheap_init();

    // paging setup
    kernel_chunk = paging_new_4gb(PAGING_IS_WRITABLE | PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL);
    paging_switch(paging_4gb_chunk_get_directory(kernel_chunk));

    // enabling paging
    enable_paging();

    // search and initialize disks (only one disk at the moment)
    disk_search_and_init();
    
    // initialize Interrupt Descriptor Table
    idt_init(); 

    // enable interrupts
    enable_interrupts();

    // testing disk streamer
    struct disk_stream* stream = diskstreamer_new(0);
    diskstreamer_seek(stream, 0x201);
    unsigned char c = 0;
    diskstreamer_read(stream, &c, 1);
    while(1);
    
}