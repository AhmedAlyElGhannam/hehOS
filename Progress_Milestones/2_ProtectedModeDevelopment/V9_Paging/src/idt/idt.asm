section .asm

extern int21h_handler
extern no_interrupt_handler

global enable_interrupts
global disable_interrupts
global idt_load
global int21h
global no_interrupt

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
    cli 
    pushad
    call no_interrupt_handler
    popad
    sti
    iret
