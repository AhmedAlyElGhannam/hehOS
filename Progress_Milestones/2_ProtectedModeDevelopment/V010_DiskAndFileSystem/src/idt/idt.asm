section .asm

extern int21h_handler
extern no_interrupt_handler

global int21h
global idt_load
global no_interrupt
global enable_interrupts
global disable_interrupts

enable_interrupts:
    sti
    ret

disable_interrupts:
    cli
    ret


idt_load:
    push ebp
    mov ebp, esp

    mov ebx, [ebp+8] ; passed address of IDTR 
    lidt [ebx] ; load IDT + give address of IDTR

    pop ebp    
    ret

; keyboard interrupt
int21h:
    cli ; to avoid nested interrupts
    pushad ; pushes all general-purpose registers
    
    call int21h_handler

    popad ; pops all general-purpose registers
    sti
    iret

no_interrupt:
    cli ; to avoid nested interrupts
    pushad ; pushes all general-purpose registers

    call no_interrupt_handler

    popad ; pops all general-purpose registers
    sti
    iret

    