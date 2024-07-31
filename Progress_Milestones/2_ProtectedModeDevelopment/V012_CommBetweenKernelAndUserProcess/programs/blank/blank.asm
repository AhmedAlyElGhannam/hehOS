[BITS 32]

section .asm

global _start

; _start:
;     push 20 ; first argument
;     push 30 ; second argument
;     mov eax, 0 ; 0 cuz this is the command I want to trigger when raising interrupt 0x80
;     int 0x80 ; raise interrupt 0x80
;     add esp, 8 ; make stack pointer point to where it initially was


_start:
    push message
    mov eax, 1 ; for print command
    int 0x80 ; raise interrupt 0x80
    add esp, 4 ; only one item was pushed into stack

    jmp $

section .data
message: db 'HELLO KERNEL FROM THE USER SIIIIIIIDE!', 0