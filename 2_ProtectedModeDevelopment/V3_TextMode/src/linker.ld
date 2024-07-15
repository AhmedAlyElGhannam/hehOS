ENTRY(_start)
OUTPUT_FORMAT(binary)

SECTIONS
{
    . = 1M; /*total addressble memory*/
     
    .text : ALIGN(4096) /*4096 is the block size in our implementation of heap *wink wink**/
    {
        *(.text) /*.text memory section*/
    }

    .rodata : ALIGN(4096)
    {
        *(.rodata) /*read-only .data memory section*/
    }

    .data : ALIGN(4096)
    {
        *(.data) /*.data memory section*/
    }

    .bss : ALIGN(4096)
    {
        *(COMMON)
        *(.bss) /*.bss memory section*/
    }

    .asm : ALIGN(4096)
    {
        *(.asm) /*added this section for assembly to avoid memory alignment issues*/
    }
    
}