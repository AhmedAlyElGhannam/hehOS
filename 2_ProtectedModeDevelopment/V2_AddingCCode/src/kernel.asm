[BITS 32]

section .asm  ; for linker sections 

global _start ; global makes this label visible to other files

CODE_SEG equ 0x08
DATA_SEG equ 0x10

_start: 
    ; setting segment registers
    mov ax, DATA_SEG
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov ebp, 0x00200000
    mov esp, ebp 

    ; enable A20 Line
    in al, 0x92  ; read from bus
    or al, 2     ; enable 2nd bit
    out 0x92, al ; output to bus

    jmp $

TIMES 512 - ($ - $$) DB 0 ; To avoid any alignment issues