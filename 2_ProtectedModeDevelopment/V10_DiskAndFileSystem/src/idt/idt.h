#ifndef IDT_H
#define IDT_H
#include <stddef.h>
#include <stdint.h>


// IDT descriptor data structure
struct idt_desc
{
    uint16_t offset_1; // offset low bits 0-15
    uint16_t selector; // selector in GDT
    uint8_t zero; // unused bytes set to 0
    uint8_t type_attr; // descriptor type and attribute (P + DPL + S + Type)
    uint16_t offset_2; // offset high bits 16-31


} __attribute__((packed));


// IDTR descriptor data structure
struct idtr_desc
{
    uint16_t limit; // size of descriptor table - 1
    uint32_t base; // base address of the start of the interrupt descriptor table

} __attribute__((packed));


void idt_init(void);

extern void enable_interrupts(void);
extern void disable_interrupts(void);

#endif