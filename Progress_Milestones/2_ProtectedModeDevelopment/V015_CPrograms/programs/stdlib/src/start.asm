[BITS 32]

global _start
extern c_start
extern hehos_exit

section .asm

_start:
    call c_start
    call hehos_exit
    ret