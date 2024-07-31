[BITS 32]

global _start ; global makes this label visible to other files
extern kernel_main ; telling this file that there is a label/function in another file
global problemo ; to access div by 0 in C code

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

    ; Enable the A20 line
    in al, 0x92 ; read from bus
    or al, 2 ; enable 2nd bit
    out 0x92, al ; output to bus

    ; Remap the master PIC
    mov al, 00010001b ; put PIC in initialization mode
    out 0x20, al ; tell master PIC
    mov al, 0x20 ; interrupt 0x20 is where master IRQ should start at
    out 0x21, al

    mov al, 00000001b
    out 0x21, al ; remap master PIC
    ; remap master PIC done

    call kernel_main ; go to C code

    jmp $

; forcing a div by zero interrupt for testing IDT 
problemo: ; equivalent to just `int 0`
    mov eax, 0;
    div eax

times 512-($ - $$) db 0 ; To avoid any alignment issues
