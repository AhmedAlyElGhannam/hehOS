; bootloader is loaded in absolute address (RAM) 0X7C00 by BIOS
ORG 0X7C00

; tell assembler that all instructions are 16-bit
BITS 16

;; this BIOS routine displays a character on the screen, advancing the cursor and scrolling the screen as necessary
    ; AH = 0Eh
    ; AL = character to write
    ; BH = page number
    ; BL = foreground color (graphics modes only)
    ; Return: Nothing
;; it happens when AH=0Eh && interrupt 10 is called
; START: 
;     MOV AH, 0Eh 
;     MOV AL, 'A'
;     MOV BX, 0
;     INT 0X10 ; this generates an interrupt corresponding with the desired routine 

;     JMP $ ; (short jump) this line creates an infinite loop (continuously jumps to this line)

start:
    MOV SI, message ; move message address to SI (source index) register
    CALL print ; call print subroutine (after it is done, will return and continue)
    JMP $

print:
    MOV BX, 0 ; set it to 0 for page number && foreground colour shenanigans
.loop:
    LODSB ; loads data pointed to by SI to AL && increments SI
    CMP AL, 0 ; if value loaded in AL is 0 (i.e.; string is done, set appropriate flags)
    JE .done ; jump to .done if flag is raised from previous comparison
    CALL print_char ; if not, go to print_char
    JMP .loop ; jump to .loop label again
.done:
    RET

print_char:
    MOV AH, 0Eh ; to execute BIOS routine to display character in terminal
    INT 0X10 ; to execute BIOS routine to display character in terminal
    RET

message: DB 'Welcome to hehOS!', 0 ; defined message


; Repeat the instruction 'db 0' 510 times; zero padding 
; memory space up to the current location in the section. 
; This is often used for aligning memory sections in assembly programs.
TIMES 510 - ($ - $$) DB 0 ; DB == define byte

; Look for a bootloader to boot by searching all storage mediums
; for boot signature "0X55AA"
; This is written as 0XAA55 instead of 0X55AA cuz intel machines are little endian
DW 0XAA55 ; DW == define word

