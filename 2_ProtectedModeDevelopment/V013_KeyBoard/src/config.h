#ifndef CONFIG_H
#define CONFIG_H

#define KERNEL_CODE_SELECTOR 0x08
#define KERNEL_DATA_SELECTOR 0x10


#define HEHOS_TOTAL_INTERRUPTS        512
#define HEHOS_HEAP_SIZE_BYTES           104857600 // 100MB heap size
#define HEHOS_HEAP_BLOCK_SIZE           4096
#define HEHOS_HEAP_ADDRESS              0x01000000 
#define HEHOS_HEAP_TABLE_ADDRESS        0x00007E00

#define HEHOS_SECTOR_SIZE               512

#define HEHOS_MAX_FILESYSTEMS           12
#define HEHOS_MAX_FILE_DESCRIPTORS      512 // max no of opened files

#define HEHOS_MAX_PATH                  108

#define HEHOS_TOTAL_GDT_SEGMENTS    6

#define HEHOS_PROGRAM_VIRTUAL_ADDRESS   0x400000
#define HEHOS_USER_PROGRAM_STACK_SIZE   (1024 * 16)
#define HEHOS_PROGRAM_VIRTUAL_STACK_ADDRESS_START   0x3FF000
// remember stack grows downwards in intel architectures aka address decreases as it grows
#define HEHOS_PROGRAM_VIRTUAL_STACK_ADDRESS_END     (HEHOS_PROGRAM_VIRTUAL_STACK_ADDRESS_START - HEHOS_USER_PROGRAM_STACK_SIZE)

// both of these are based on real offsets in gdt 
#define USER_DATA_SEGMENT   0x23
#define USER_CODE_SEGMENT   0x1B

#define HEHOS_MAX_PROGRAM_ALLOCATIONS   1024

#define HEHOS_MAX_PROCESSES 12

#define HEHOS_MAX_ISR80H_COMMANDS 1024

#endif