[BITS 32]

section .asm

global paging_load_directory
global enable_paging

paging_load_directory:
    push ebp
    mov ebp, esp

    mov eax, [ebp+8] ; extract passed pointer to page directory
    mov cr3, eax ; page directory address should be stored in control register 3

    pop ebp
    ret

enable_paging:
    push ebp
    mov ebp, esp

    mov eax, cr0 ; cannot change cr0 directly
    or eax, 0x80000000 ; set bit 31 to 1 to enable paging
    mov cr0, eax

    pop ebp
    ret
