ORG 0X7C00

; tell assembler that all instructions are 16-bit
BITS 16

; set CS && DS offsets for GDT
CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start

;; Add BIOS Parameter Block to prevent BIOS from corrupting this code
_start:
    JMP SHORT start
    NOP

TIMES 33 DB 0 ;; 33 bytes as described in BIOS Parameter Block table

start:
    JMP 0X7C0:step2

;; copy sector into memory
; AH = 02h
; AL = number of sectors to read (must be nonzero)
; CH = low eight bits of cylinder number
; CL = sector number 1-63 (bits 0-5)
; high two bits of cylinder (bits 6-7, hard disk only)
; DH = head number
; DL = drive number (bit 7 set for hard disk)
; ES:BX -> data buffer

; Return:
; CF set on error
; if AH = 11h (corrected ECC error), AL = burst length
; CF clear if successful
; AH = status (see #00234)
; AL = number of sectors transferred (only valid if CF set for some
; BIOSes)


step2:
    CLI ; clear interrupts
    ;; has to clear/disable interrupts to change some segment registers
    ; set data/extra segment
    MOV AX, 0X00
    MOV DS, AX
    MOV ES, AX
    ; set stack segment
    MOV SS, AX
    MOV SP, 0X7C00
    ;;
    STI ; enable interrupts
    
; load protected mode
.load_protected:
    CLI ; disable interrupts due to critical section
    lgdt[gdt_descriptor] ; load global descriptor table
    MOV eax, cr0 ; copy content of cr0
    OR eax, 0x1 ; set Protection Enable bit in Control Register 0
    MOV cr0, eax ; mov updated value back to cr0
    jmp CODE_SEG:load32

; GDT
gdt_start:


; GDT template values to-be filled with default values
gdt_null:
    dd 0x00
    dd 0x00

; offset 0x8
gdt_code:        ; CS points to this
    ; all this is explained in detail in GDT page on OSdev
    dw 0xFFFF    ; segment limit first 0-15 bits
    dw 0         ; base first 0-15 bits
    db 0         ; base 16-23 bits
    db 0x9A      ; access byte (fields include: present bit, privilage bit,...)
    db 11001111b ; high 4 bit and low 4 bit flags
    db 0         ; base 24-31 bits

; offset 0x10
gdt_data:        ; should be linked to DS, SS, ES, FS, GS
    ; all this is explained in detail in GDT page on OSdev
    dw 0xFFFF    ; segment limit first 0-15 bits
    dw 0         ; base first 0-15 bits
    db 0         ; base 16-23 bits
    db 0x92      ; access byte (fields include: present bit, privilage bit,...)
    db 11001111b ; high 4 bit and low 4 bit flags
    db 0         ; base 24-31 bits

gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start-1 ; descriptor size
    dd gdt_start ; offset

[BITS 32]
load32: 
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

TIMES 510 - ($ - $$) DB 0 
dw 0XAA55 



