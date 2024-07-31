ORG 0x7c00

; tell assembler that all instructions are 16-bit
BITS 16

; set CS && DS offsets for GDT
CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start

jmp short start
nop

; FAT16 header
OEMIdentifier           db 'hehOS   ' ; 8 bytes PADDED WITH SPACES!
BytesPerSector          dw 0x200 ; 512 bytes per sector
SectorsPerCluster       db 0x80 ; 128 sectors per cluster
ReservedSectors         dw 200 ; reserved for kernel space
FATCopies               db 0x02 ; original + backup copy
RootDirEntries          dw 0x40
NumSectors              dw 0x00 ; not used
MediaType               db 0xF8
SectorsPerFat           dw 0x100
SectorsPerTrack         dw 0x20
NumberOfHeads           dw 0x40
HiddenSectors           dd 0x00
SectorsBig              dd 0x773594

; Extended BPB (Dos 4.0)
DriveNumber             db 0x80
WinNTBit                db 0x00
Signature               db 0x29
VolumeID                dd 0xD105
VolumeIDString          db 'hehOS BOOT '
SystemIDString          db 'FAT16   '


start:
    jmp 0:step2

step2:
    cli ; Clear Interrupts
    ;;
    mov ax, 0x00
    mov ds, ax
    mov es, ax

    mov ss, ax
    mov sp, 0x7C00
    ;;
    sti ; Enables Interrupts

; load protected mode
.load_protected:
    cli ; disable interrupts due to critical section
    lgdt[gdt_descriptor] ; load global descriptor table
    mov eax, cr0 ; copy content of cr0
    or eax, 0x1 ; set Protection Enable bit in Control Register 0
    mov cr0, eax ; mov updated value back to cr0
    jmp CODE_SEG:load32
    
; GDT
gdt_start:

; GDT template values to-be filled with default values
gdt_null:
    dd 0x0
    dd 0x0

; offset 0x8
gdt_code:     ; CS SHOULD POINT TO THIS
    dw 0xFFFF ; Segment limit first 0-15 bits
    dw 0      ; Base first 0-15 bits
    db 0      ; Base 16-23 bits
    db 0x9A   ; Access byte
    db 11001111b ; High 4 bit flags and the low 4 bit flags
    db 0        ; Base 24-31 bits

; offset 0x10
gdt_data:      ; DS, SS, ES, FS, GS
    dw 0xFFFF ; Segment limit first 0-15 bits
    dw 0      ; Base first 0-15 bits
    db 0      ; Base 16-23 bits
    db 0x92   ; Access byte
    db 11001111b ; High 4 bit flags and the low 4 bit flags
    db 0        ; Base 24-31 bits

gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start-1 ; descriptor size
    dd gdt_start ; offset
 
 [BITS 32]
 load32:
    mov eax, 1 ; starting sector
    mov ecx, 100 ; number of sectors
    mov edi, 0x0100000 ; address to load
    call ata_lba_read
    jmp CODE_SEG:0x0100000

; LBA == Linear Block Address
; the memory access implementation that will be replaced later
ata_lba_read:
    mov ebx, eax, ; backup LBA
    ; send the highest 8 bits of LBA to hard disk controller
    shr eax, 24
    or eax, 0xE0 ; select master drive
    mov dx, 0x1F6
    out dx, al
    ; sending highest 8 bits done

    ; send total sectors to read
    mov eax, ecx
    mov dx, 0x1F2
    out dx, al
    ; sending total sectors done

    ; send more stuff to LBA
    mov eax, ebx ; restore the backup LBA
    mov dx, 0x1F3
    out dx, al
    ; send more stuff to LBA doneA

    ; send more stuff to LBA (the sequel)
    mov dx, 0x1F4
    mov eax, ebx ; restore the backup LBA
    shr eax, 8
    out dx, al
    ; send more stuff to LBA (the sequel) done

    ; send upper 16 bits of the LBA
    mov dx, 0x1F5
    mov eax, ebx ; restore the backup LBA
    shr eax, 16
    out dx, al
    ; send upper 16 bits to LBA done

    mov dx, 0x1f7
    mov al, 0x20
    out dx, al

; read all sectors in mem
.next_sector:
    push ecx

; check if reread is required
.try_again:
    mov dx, 0x1f7
    in al, dx
    test al, 8
    jz .try_again

    ; read 256 words at a time
    mov ecx, 256 ; number of read operations for 'rep' command
    mov dx, 0x1F0 ; address of input port for 'insw' operation
    rep insw ; repeatedly read and store it in 0x0100000
    pop ecx
    loop .next_sector
    ; End of reading sectors into memory
    ret

times 510-($ - $$) db 0 ; zero padding to make it a full sector

dw 0xAA55; boot signature
