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

;;
    ; This is basically how you define an interrupt
    ; here I overriden the behaviour of INTERRUPT 0 (which is division by 0)
    ; and made it print the character 'A' instead
;;
handle_zero:
    MOV AH, 0Eh
    MOV AL, 'A'
    MOV BX, 0x00
    INT 0x10
    IRET
;; override the behaviour of interrupt 1
handle_one:
    MOV AH, 0Eh
    MOV AL, 'V'
    MOV BX, 0x00
    INT 0x10
    IRET

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
        ; change interrupt vector table ###
    MOV WORD[SS:0x00], handle_zero ; offset of INT0 (force use of SS)
    MOV WORD[SS:0x02], 0x7C0;      ; segment of INT0

    INT 0 ; triggers interrupt 0

    ; MOV AX, 0x00 
    ; DIV AX    ;; THIS ALSO TRIGGERS INTERRUPT 0
    ;;

    ;;
        ; change interrupt vector table ###
    MOV WORD[SS:0x04], handle_one  ; offset of INT1
    MOV WORD[SS:0x06], 0x7C0;      ; segment of INT1

    INT 1 ; triggers interrupt 0
    ;;

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

