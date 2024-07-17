#include "../kernel.h"
#include "idt.h"
#include "../config.h"
#include "../memory/memory.h"
#include "../io/io.h"

struct idt_desc idt_descriptors[HEHOS_TOTAL_INTERRUPTS];
struct idtr_desc idtr_descriptor;

extern void idt_load(struct idtr_desc* ptr);
extern void int21h(void);
extern void no_interrupt(void);

void int21h_handler(void)
{
    print("Keyboard Pressed!\n");
    outb(0x20, 0x20);
}

void no_interrupt_handler(void)
{
    outb(0x20, 0x20);
}

void idt_zero(void)
{
    print("Divide By Zero Error\n");
}

void idt_set(int interrupt_num, void* address)
{
    struct idt_desc* desc = &idt_descriptors[interrupt_num];
    desc->offset_1 = (uint32_t)address & 0x0000FFFF;
    desc->selector = KERNEL_CODE_SELECTOR;
    desc->zero = 0x00;
    desc->type_attr = 0xEE;// ring 3 -- interrupt gate 32-bit -- P = 1 -- S = 0 for interrupts
    desc->offset_2 = (uint32_t) address >> 16;
}

void idt_init(void)
{
    memset(idt_descriptors, 0, sizeof(idt_descriptors));
    idtr_descriptor.limit = sizeof(idt_descriptors) - 1;
    idtr_descriptor.base = (uint32_t) idt_descriptors;
    idt_load(&idtr_descriptor);

    for (int i = 0; i < HEHOS_TOTAL_INTERRUPTS; i++)
    {
        idt_set(i, no_interrupt);
    }

    idt_set(0, idt_zero);
    idt_set(PIC_TIMER_INT, int21h);
}