#ifndef IDT_H
#define IDT_H
#include <stddef.h>
#include <stdint.h>

#define PIC_MASTER_ADD  0x20
#define PIC_MASTER_ACK  0x20
#define PIC_SLAVE_ADD   0xA0

#define PIC_TIMER_INT    (PIC_MASTER_ADD + 0)
#define PIC_KEYBRD_INT   (PIC_MASTER_ADD + 1)
#define PIC_CASC_INT     (PIC_MASTER_ADD + 2)
#define PIC_COM2_INT     (PIC_MASTER_ADD + 3)
#define PIC_COM1_INT     (PIC_MASTER_ADD + 4)
#define PIC_LPT2_INT     (PIC_MASTER_ADD + 5)
#define PIC_FLPDISK_INT  (PIC_MASTER_ADD + 6)
#define PIC_LPT1_INT     (PIC_MASTER_ADD + 7)

#define PIC_RTCLK_INT    (PIC_SLAVE_ADD + 0)
#define PIC_PERIPH1_INT  (PIC_SLAVE_ADD + 1)
#define PIC_PERIPH2_INT  (PIC_SLAVE_ADD + 2)
#define PIC_PERIPH3_INT  (PIC_SLAVE_ADD + 3)
#define PIC_MOUSEPS_INT  (PIC_SLAVE_ADD + 4)
#define PIC_COPROC_INT   (PIC_SLAVE_ADD + 5)
#define PIC_ATAPRIM_INT  (PIC_SLAVE_ADD + 6)
#define PIC_ATASEC_INT   (PIC_SLAVE_ADD + 7)

// forward declaration for this struct
struct interrupt_frame;

// defining ISR command function pointer type
typedef void*(*ISR80H_COMMAND)(struct interrupt_frame* frame);

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

struct interrupt_frame
{
    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;
    uint32_t reserved;
    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;
    uint32_t ip;
    uint32_t cs;
    uint32_t flags;
    uint32_t esp;
    uint32_t ss;
}__attribute__((packed));

void idt_init(void);
void* isr80h_handler(int command, struct interrupt_frame* frame); 
void isr80h_register_command(int command_id, ISR80H_COMMAND command);
extern void enable_interrupts(void);
extern void disable_interrupts(void);

#endif