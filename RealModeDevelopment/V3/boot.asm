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
    MOV SI, message 
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

message: DB 'Welcome to hehOS!', 0 ; defined message

TIMES 510 - ($ - $$) DB 0 
DW 0XAA55 

