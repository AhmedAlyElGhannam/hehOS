[BITS 32]

section .asm

global task_return
global restore_general_purpose_registers
global user_registers

; extern void task_return(struct registers* regs);
task_return:
    mov ebp, esp
    ; push ds (ss will be fine and no need to push it)
    ; push stack address
    ; push the flags
    ; push cs
    ; push ip

    mov ebx, [ebp+4] ; acccess the passed structure
    push dword [ebx+44] ; push data/stack selector
    push dword [ebx+40] ; push stack pointer
    pushf ; push flags
    pop eax ; pop flags
    or eax, 0x200 ; enable interrupts upon calling iret
    push eax ; re-push flags back
    push dword [ebx+32] ; push code segment
    push dword [ebx+28] ; push ip to execute
    
    ; setup some segment regs
    mov ax, [ebx+44] 
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    push dword [ebx+4]
    call restore_general_purpose_registers
    add esp, 4

    iretd ; leave kernel land and execute user... land!


; extern void restore_general_purpose_registers(struct registers* regs);
restore_general_purpose_registers:
    push ebp
    mov ebp, esp
    mov ebx, [ebp+8]
    mov edi, [ebx]
    mov esi, [ebx+4]
    mov ebp, [ebx+8]
    mov edx, [ebx+16]
    mov ecx, [ebx+20]
    mov eax, [ebx+24]
    mov ebx, [ebx+12]
    pop ebp
    ret

; extern void user_registers(void);
user_registers:
    mov ax, 0x23 ; changes selector value to user data segment
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    ret