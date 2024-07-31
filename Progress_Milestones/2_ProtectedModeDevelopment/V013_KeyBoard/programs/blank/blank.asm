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

_loop:
    call getkey
    push eax
    mov eax, 3 ; command 3 putchar
    int 0x80 ; raise interrupt 0x80
    add esp, 4 ; only one item was pushed into stack
    jmp _loop ; keep looping to keep printing characters

getkey:
    mov eax, 2 ; command 2 for getting key
    int 0x80 ; raise interrupt 0x80
    cmp eax, 0x00 
    je getkey ; repeat if input char is null
    ret


section .data
message: db 'HELLO KERNEL FROM THE USER SIIIIIIIDE!', 0