#include "kernel.h"
#include "idt/idt.h"
#include "io/io.h"
#include "memory/heap/kheap.h"

uint16_t* video_mem = 0;
uint16_t terminal_row = 0;
uint16_t terminal_col = 0;

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


size_t strlen(const char* str)
{
    size_t len = 0;

    while(str[len])
    {
        len++;
    }

    return len;
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

void kernel_main(void)
{
    terminal_initialize();

    print_sussy_bakka();

    // initialize heap
    kheap_init();

    // initialize Interrupt Descriptor Table
    idt_init(); 


    // added div by 0 to test IDT functionality
    // problemo();

    // added out operation to test i/o functionality
    // outb(0x60, 0xFF);

    void* ptr = kmalloc(50);
    void* ptr2 = kmalloc(5000);
    void* ptr3 = kmalloc(5000);
    kfree(ptr);
    void* ptr4 = kmalloc(50);
    void* ptr5 = kmalloc(50);
    if (ptr || ptr2 || ptr3 || ptr4 || ptr5)
    {

    }
}