section .asm

global idt_load
idt_load:
    push ebp
    mov ebp, esp

    mov ebx, [ebp+8] ; passed address of IDTR 
    lidt [ebx] ; load IDT + give address of IDTR

    pop ebp
    ret