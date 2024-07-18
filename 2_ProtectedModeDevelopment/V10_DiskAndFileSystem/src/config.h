#ifndef CONFIG_H
#define CONFIG_H

#include <stddef.h>
#include <stdint.h>

#define KERNEL_CODE_SELECTOR    0x08
#define KERNEL_DATA_SELECTOR    0x10

#define HEHOS_TOTAL_INTERRUPTS      512
#define HEHOS_HEAP_SIZE_BYTES       104857600 // 100MB heap size
#define HEHOS_HEAP_BLOCK_SIZE       4096
#define HEHOS_HEAP_ADDRESS          0x01000000
#define HEHOS_HEAP_TABLE_ADDRESS    0x00007E00

#define HEHOS_SECTOR_SIZE       512


#endif