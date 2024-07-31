section .asm

global insb

insb:
    push ebp
    mov ebp, esp

    xor eax, eax ; make eax 0s
    mov edx, [ebp+8] ; store the passed value (port) in edx
    in al, dx ; input the data at the port in dx to al

    pop ebp
    ret ; return al
    ; return value is always in eax for assem/C interactions
