section .asm

; all deez are C functions in outside files
extern int21h_handler
extern no_interrupt_handler
extern isr80h_handler
extern interrupt_handler

global idt_load
global no_interrupt
global enable_interrupts
global disable_interrupts
global isr80h_wrapper
global interrupt_pointer_table

enable_interrupts:
    sti
    ret

disable_interrupts:
    cli
    ret


idt_load:
    push ebp
    mov ebp, esp

    mov ebx, [ebp+8] ; passed address of IDTR 
    lidt [ebx] ; load IDT + give address of IDTR

    pop ebp    
    ret

no_interrupt:
    cli ; to avoid nested interrupts
    pushad ; pushes all general-purpose registers to stack

    call no_interrupt_handler

    popad ; pops all general-purpose registers from stack
    sti
    iret

;; creating a macro for all 512 isr80h interrupt wrappers
%macro interrupt 1
    global int%1
    int%1:
        cli ; to avoid nested interrupts
        ; since this interrupt will be called from user land
        ; the processor will push the same stuff that enabled it 
        ; to get to user land
        ;; PUSHED AUTOMATICALLY UPON ENTRY BY CPU (deez represent execution context)
        ;; Interrupt Frame start ;;
        ;; uint32_t ip
        ;; uint32_t cs
        ;; uint32_t flags
        ;; uint32_t sp
        ;; uint32_t ss
        ;; Interrupt Frame end ;;
        pushad ; pushes all general-purpose registers to stack
        push esp ; push stack pointer so that we are pointing to interrupt frame
        push dword %1 ; pushing interrupt number to execute the correct routine
        call interrupt_handler
        add esp, 8 ; point to the address prior to pushing esp (+4) and %1 (+4) to pop the right values
        popad ; restore general purpose registers for user land
        sti
        iret
%endmacro

;; making a loop using macro to call the interrupt macro 512 times!
%assign i 0
%rep 512  
    interrupt i
%assign i i+1
%endrep

isr80h_wrapper:
    cli ; to avoid nested interrupts
    ; since this interrupt will be called from user land
    ; the processor will push the same stuff that enabled it 
    ; to get to user land
    ;; PUSHED AUTOMATICALLY UPON ENTRY BY CPU (deez represent execution context)
    ;; Interrupt Frame start ;;
    ;; uint32_t ip
    ;; uint32_t cs
    ;; uint32_t flags
    ;; uint32_t sp
    ;; uint32_t ss
    ;; Interrupt Frame end ;;
    pushad ; pushes all general-purpose registers to stack

    push esp ; push stack pointer so that we are pointing to interrupt frame
    push eax ; contains the command that this code should invoke (like 1 for print)
    call isr80h_handler
    mov dword[tmp_res], eax ; a C function's return data will be stored in eax so move it to this label
    add esp, 8 ; point to the address prior to pushing esp (+4) and eax (+4) to pop the right values

    popad ; restore general purpose registers for user land
    mov eax, [tmp_res] ; this is done to ensure the return value is not corrupted by writing in eax by mistake
    sti
    iret


section .data ; NEEDS TO BE IN DATA SECTION
; defining the temporary variable to store C function return data
tmp_res: dd 0

;; each entry is basically the address of the label int%1
%macro interrupt_array_entry 1
    dd int%1
%endmacro

;; this is the array of pointers to interrupt wrappers
interrupt_pointer_table:
%assign i 0
%rep 512
    interrupt_array_entry i
%assign i i+1
%endrep

