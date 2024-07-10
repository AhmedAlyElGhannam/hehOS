ORG 0X0000

; tell assembler that all instructions are 16-bit
BITS 16

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
    MOV AX, 0X7C0
    MOV DS, AX
    MOV ES, AX
    ; set stack segment
    MOV AX, 0X00
    MOV SS, AX
    MOV SP, 0X7C00
    ;;
    STI ; enable interrupts
    
    ;;
    ; sector shenanigans (Using CHS---Will use LBA later)
    MOV AH, 02h ; read sector command
    MOV AL, 1   ; number of sectors to be read
    MOV CH, 0   ; low eight bits of cylinder number
    MOV CL, 2   ; read sector 2
    MOV DH, 0   ; head number
    MOV BX, buffer
    INT 0x13    ; call subroutine 0x13
    ;;
    JC error    ; jump if carry flag is raised (CF = 1 @ errors)
    
    MOV SI, buffer
    CALL print
    
    JMP $

error:
    MOV SI, error_message
    CALL print
    JMP $

print:
    MOV BX, 0 
.loop:
    LODSB 
    CMP AL, 0 
    JE .done 
    CALL print_char 
    JMP .loop 
.done:
    RET

print_char:
    MOV AH, 0Eh 
    INT 0X10 
    RET


error_message: db 'Failed to load sector', 0

TIMES 510 - ($ - $$) DB 0 
DW 0XAA55 

buffer:

