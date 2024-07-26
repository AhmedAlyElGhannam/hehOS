#include "../kernel.h"
#include "idt.h"
#include "../config.h"
#include "../memory/memory.h"
#include "../io/io.h"
#include "../task/task.h"

struct idt_desc idt_descriptors[HEHOS_TOTAL_INTERRUPTS];
struct idtr_desc idtr_descriptor;

// this is implemented in asm to keep track of all the interrupts addresses
extern void* interrupt_pointer_table[HEHOS_TOTAL_INTERRUPTS];
static ISR80H_COMMAND isr80h_commands[HEHOS_MAX_ISR80H_COMMANDS];

extern void idt_load(struct idtr_desc* ptr);
extern void int21h(void);
extern void no_interrupt(void);
extern void* isr80h_wrapper();

void interrupt_handler(int interrupt, struct interrupt_frame* frame)
{
    outb(PIC_MASTER_ADD, PIC_MASTER_ACK);
}

void no_interrupt_handler(void)
{
    outb(0x20, 0x20);
}

void idt_zero()
{
    print("Divide by zero error\n");
}

void idt_set(int interrupt_no, void* address)
{
    struct idt_desc* desc = &idt_descriptors[interrupt_no];
    desc->offset_1 = (uint32_t) address & 0x0000ffff;
    desc->selector = KERNEL_CODE_SELECTOR;
    desc->zero = 0x00;
    desc->type_attr = 0xEE; // ring 3 -- interrupt gate 32-bit -- P = 1 -- S = 0 for interrupts
    desc->offset_2 = (uint32_t) address >> 16;
}

void idt_init()
{
    memset(idt_descriptors, 0, sizeof(idt_descriptors));
    idtr_descriptor.limit = sizeof(idt_descriptors) -1;
    idtr_descriptor.base = (uint32_t) idt_descriptors;

    for (int i = 0; i < HEHOS_TOTAL_INTERRUPTS; i++)
    {
        idt_set(i, interrupt_pointer_table[i]);
    }

    idt_set(0, idt_zero);
    idt_set(0x80, isr80h_wrapper);


    // load the interrupt descriptor table
    idt_load(&idtr_descriptor);
}

// this function basically sets a new command to be handled when int 0x80 is raised
void isr80h_register_command(int command_id, ISR80H_COMMAND command)
{
    if (command_id < 0 || command_id >= HEHOS_MAX_ISR80H_COMMANDS)
    {
        panic("isr80h_register_command: Command is Out of Bounds!\n");
    }

    // if command is already taken/exists
    if (isr80h_commands[command_id])
    {
        // cannot have two commands share the same id
        panic("isr80h_register_command: Attempting to Overwrite an Existing Command!\n");
    }

    isr80h_commands[command_id] = command;
}

void* isr80h_handle_command(int command, struct interrupt_frame* frame)
{
    void* result = 0;

    // if out of bounds
    if (command < 0 || command >= HEHOS_MAX_ISR80H_COMMANDS)
    {
        return 0; // invalid command
    }

    // fetch command handler address from supported list of handlers
    ISR80H_COMMAND command_func = isr80h_commands[command];
    if (!command_func)
    {
        return 0; // command not supported
    }

    result = command_func(frame);

    return result;
}

void* isr80h_handler(int command, struct interrupt_frame* frame)
{
    void* res = 0;
    kernel_page();
    task_current_save_state(frame);
    res = isr80h_handle_command(command, frame);
    task_page();
    return res;
}