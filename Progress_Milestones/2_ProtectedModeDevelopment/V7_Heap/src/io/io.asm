section .asm

global insb
global insw
global outb
global outw

insb:
    push ebp
    mov ebp, esp

    xor eax, eax ; make eax 0s
    mov edx, [ebp+8] ; store the passed value (port) in edx
    in al, dx ; input the data at the port in dx to al

    pop ebp
    ret ; return al
    ; return value is always in eax for assem/C interactions

insw:
    push ebp
    mov ebp, esp

    xor eax, eax
    mov edx, [ebp+8];
    in ax, dx

    pop ebp
    ret

outb:
    push ebp
    mov ebp, esp

    mov eax, [ebp+12]
    mov edx, [ebp+8]
    out dx, al

    pop ebp
    ret

outw:
    push ebp
    mov ebp, esp

    mov eax, [ebp+12]
    mov edx, [ebp+8]
    out dx, ax

    pop ebp
    ret