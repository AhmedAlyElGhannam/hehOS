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
#include "terminal/terminal.h"

void panic(const char* msg)
{
    print(msg);
    while(1);
}

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

    // enable interrupts
    // enable_interrupts();

    // initialize all of system's keyboards 
    keyboard_init();

    struct process* process = 0;
    int res = process_load_switch("0:/shelly.elf", &process);
    if (res != HEHOS_ALL_OK)
    {
        panic("Failed to Load shelly.elf!\n");
    }

    task_run_first_ever_task();
    
    while(1);
}