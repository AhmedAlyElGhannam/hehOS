section .asm

global tss_load

tss_load:
    push ebp
    mov ebp, esp
    mov ax, [ebp+8] ; passed int tss_segment variable
    ltr ax
    pop ebp
    ret
