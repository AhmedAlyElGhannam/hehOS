# Protected Mode -- Introduction
## What?
A processor state in x86 architecture which gives access many features such as `memory protection` & `4GB address space.`

## Features
1. Provides memory & hardware protection.
1. Different memory schemes.
1. 4GB of addressable memory.

### Memory & Hardware Protection
There are multiple levels of privilages that software can have when run on hardware:
    1. Ring 0---Kernel, most privilaged.
    1. Ring 1 & 2---Device Drivers, enable interfacing with hardware.
    1. Ring 3---User Application, least privilaged; needs **Interrupts** to access kernel level commands.

### Different Memory Schemes
Ways to traverse, deal with, and access memory; like:
    1. Selectors---Code Selector, Data Selector, Extra Selector, Stack Selector,... etc.
        1. Segmentation registers `CS - SS - DS - ES` become selector registers.
        1. Selector registers **point** to data structures that describe **memory ranges** and the permissions **(ring level)** required to access a given range.
        1. In order to set the *base* and *limit* correctly, use **bitwise logic operations.**
    1. Paging---Remaping memory addresses into **virtual** addresses.
        1. Pointing to a virtual memory address can address an entirely different address in actual memory.
        1. Memory protecc is easier to control/
        1. Paging is the memory scheme-of-choice for most OS's & kernels.

### 4GB of Addressable Memory
1. Gain access to 32-bit instructions: meaning that 32-bit registers can be used.
1. Up to 4GB's of memory are addressable. The limit of 1MB of memory in Real Mode is no longer here.

---------------------------------------------------------

# Setting GCC Cross Compiler
1. Echo the following lines into `.bashrc` then source `.bashrc`.
    ```
    export PREFIX="$HOME/opt/cross"
    export TARGET=i686-elf
    export PATH="$PREFIX/bin:$PATH"
    ```
1. Download `binutils-v2.35.tar.xz` from this link: https://ftp.gnu.org/gnu/binutils/binutils-2.35.tar.xz
1. Extract `binutils-v2.35.tar.xz` into your $HOME directory.
1. Create a new folder `src` in $HOME and move the `binutils-v2.35` folder into it.
1. Run the bash script `binutils.sh`.
1. Download `gcc-10.2.0` from this link: https://ftp.gnu.org/gnu/gcc/gcc-10.2.0/gcc-10.2.0.tar.gz
1. Extract `gcc-10.2.0.tar.gz` into the $HOME/src folder created before.
1. Run the script `gcc-10.sh`.
1. To make sure `gcc` was installed successfully, run:
    ```
    $HOME/opt/cross/bin/$TARGET-gcc --version
    ```

---------------------------------------------------------

# Switching To Protected Mode
A few things must be done before switching to Protected Mode:
1. Disable interrupts---since this is a critical section in the bootloader.
1. Enable the A20 Line (It is disabled to ensure backward compatibility when running older software.)---this allows the cpu to ditch the 8086 way of only accessing 1MB of memory. 
1. Load the Global Descriptor Table (GDT)---a data structure that defines the characteristics of memory segments like size, base address, and access privilages (write and executable). **GDT is basically a pointer (with a size and offset) to the actual table. It has loads of entry fields but they will be set to their default values since my kernel uses paging; which does not care about their values.**

```
; Added Protected Mode Switching Shenanigans

```

```
gdb
target remote | qemu-system-x86_64 -hda ./boot.bin -S -gdb stdio
c ### aka continue
```


--------------------
```
gdb
add-symbol-file ./build/kernelfull.o 0x100000
y
break _start
target remote | qemu-system-x86_64 -S -gdb stdio -hda ./bin/os.bin
c
c
```

To fix alignment issues:
1. pad kernel.asm to be 512 bytes aka 1 sector
1. align memory sections in linker file at 4096 bytes

---------------------------------------

# Text Mode
## Features
1. Allows programmer to write ASCII characters in video memory.
1. Supports **16** unique colours.
1. No need to set individual screen pixels for printing characters. **kalm**

## Writing ASCII Characters to Video Memory
1. ASCII characters are written into memory starting at address **0xB8000** for coloured displays.
1. ASCII characters are written into memory starting at the address **0xB0000** for monochrome displays.
1. Each ASCII character written to this memory has its pixel equivalent outputted to the monitor.

## Text Mode Supported Colours
1. 0  -> Black
1. 1  -> Blue
1. 2  -> Green
1. 3  -> Cyan
1. 4  -> Red
1. 5  -> Purple
1. 6  -> Brown
1. 7  -> Gray
1. 8  -> Dark Gray
1. 9  -> Light Blue
1. 10 -> Light Green
1. 11 -> Light Cyan
1. 12 -> Light Red
1. 13 -> Light Purple
1. 14 -> Yellow
1. 15 -> White

## Characters Are Printed Without Setting Individual Screen Pixels

While in Text Mode, the video card will take a given ASCII value and automatically reference it in a font table to output the correct pixels for the given character---like the letter 'A' for example.

### Example: Writing 'A' in Black at (0,0) & 'B' in White at (0,1)
```
0xB8000 = 'A'
0xB8001 = 0x00
0xB8002 = 'B'
0xB8003 = 0x0F
```
**OR,** both bytes (char && colour) are addressed at the same time. But, beware that `i386` is little endian and thus if the value `0x650F` must be written in memory **it is passed as 0x0065.**

```
uint16_t* video_mem = (uint16_t*)(0xB8000);
video_mem[0] = 0x0065; // 65 for 'A' && 00 for black
```

### AmogOS Logo For... Reasearch Purposes ( ͡° ͜ʖ ͡°)

                    _______________
                   /               \
                  /                 \
                 /     _________     \
                |     /         \     \
               /     (           )    |
               |      \_________/     |
              /                       \
              |                        |
              |                        |
             /                         |
             |                         |
             |                         |
            /                          |
            |                          |
            |        ________          |
      _____/        /        \         |
     /              |        |         |
     |             /   _____/          |
     \____________/   /               _|
                      \            __/
                       \__________/


# Interrupt Descriptor Table
It is Protected Mode's equivalent of Interrupt Vector Table.
1. It describes how interrupts are invoked in Protected Mode.
1. It can be mapped/defined anywhere in memory.
1. It is inherently different from Interrupt Vector Table.

## Interrupt Invokation Description in Protected Mode
1. Similar to Interrupt Vector Table, Interrupt Descriptor Table describes how interrupts are setup in the CPU so that if someone causes an `int 5` it will invoke the code for interrupt 5 as described by the Interrupt Descriptor Table.
1. Each entry is 64 bit, with multiple fields, including:
    1. Offset (Offset 16-31) -> bit 48-63: the higher part of the offset to execute.
    1. P (Present)           -> bit 47: should be set to 0 for unused interrupts.
    1. DPL (Descriptor Privilage Level) -> bit 45-46: ring level the CPU requires to call this interrupt.
    1. S (Storage Segment) -> bit 44: should be set to 0 for trap gates.
    1. Type (Gate Type) -> bit 40-43: type of gate this interrupt is treated as.
    1. 0 (Unused 0-7) -> bit 32-39: unused bits in this structure.
    1. Selector (Selector 0-15) -> bit 16-31: the selector this interrupt is bounded to aka the kernel Code Selector.
    1. Offset (Offset 0-15) -> bit 0-15: the lower part of the offset to execute.
1. Interrupt Gate Types:
    1. Task Gate (32-bit): References Task State Segment (TSS) descriptor and can assist in multi-tasking when exceptions occur.
    1. Interrupt Gate (16-bit & 32-bit): Used for interrupts that the programmer wants to invoke in their code.
    1. Trap Gate (16-bit & 32-bit): Like Interrupt Gates but used for exceptions. Additionally, they disable interrupts on entry and re-enable them on an `iret` instruction.
1. Interrupts are referenced by index using the Interrupt Descriptor Array---index 0 for interrupt 0, index 1 for interrupt 1 and so on.
1. Interrupt Descriptor Table Register (IDTR) is a 48-bit data structure that contains 2 fields:
    1. Limit (bit 0-15): length of Interrupt Descriptor Table minus 1.
    1. Base (bit 16-47): the address of the interrupt descriptor table.
1. IDT is loaded by invoking the `lidt` instruction and passing a pointer to IDTR.


# Bro_Tip
For any assembly function, it is good practice to create a stack frame like this:
```
fun_name:
    push ebp
    mov ebp, esp
    ;; function body
    pop ebp
    ret
```

# Programmable Interrupt Controller
## Characteristics
1. Allows hardware to interrupt the processor's state.
1. `programmable`
1. Requires interrupt acknowledgement.

## Ability for Hardware to Interrupt CPU
### Standard ISA IRQ's:
    1. 0  -> Timer Interrupt (useful for multi-tasking)
    1. 1  -> Keyboard Interrupt
    1. 2  -> Cascade Interrupt (used internally by 2 PIC's -- never used)
    1. 3  -> COM2 (if enabled)
    1. 4  -> COM1 (if enabled) 
    1. 5  -> LPT2 (if enabled) (Line Print Terminal---parallel port used for printers)
    1. 6  -> Floppy Disk
    1. 7  -> LPT1 (if enabled)
    1. 8  -> CMOS Real-Time Clock (if enabled)
    1. 9  -> Free for Peripherals/Legacy SCSI (Small Computer System Interface)/NIC (Network Interface Card)
    1. 10 -> Free for Peripherals/Legacy SCSI (Small Computer System Interface)/NIC (Network Interface Card)
    1. 11 -> Free for Peripherals/Legacy SCSI (Small Computer System Interface)/NIC (Network Interface Card)
    1. 12 -> PS2 Mouse (old green-ish circular mouse port)
    1. 13 -> FPU/Co-Processor/Inter-Processor
    1. 14 -> Primary ATA Hard Disk
    1. 15 -> Secondary ATA Hard Disk

### Understanding IRQ's
    1. IRQ's are **mapped** to a starting interrupt (say 0x20 for example).
    1. IRQ 0 would then be interrupt 0x20, IRQ 1 would be interrupt 0x21 and so on.
    1. By default, some IRQ's are mapped to interrupts 8-15. **This is a problem:** these interrupts are reserved in Protected Mode for Exceptions. So, it is required to remap PIC.

## Programmable Interrupt Controllers Configuration
1. The system has two PIC's: one for **slave ports** and the other for **master ports**.
1. The master handles IRQ 0-7. (uses port 0x20 && 0x21 for control ports)
1. The slave handles IRQ 8-15. (uses port 0xA0 && 0xA1 for control ports)

## Interrupt Acknowledgement
Acknowledgement is fairly simple: just an `out` operation to the control port with the sent data the same value as port number.
Ex: port 0x20 and send data value 0x20.

# Heap
What is it?
1. A giant memory region that can be shared in a controlled manner.
1. A programmer can ask the heap for memory and tell the heap when they are done with that memory.
1. Heap implementations are essentially system memory management.

## C Programming Language Memory Operations
1. In C, any memory address can be pointed to regardless of the programmer's ability to access it or not.
1. Malloc in C returns a memory address of memory that the programmer can write to---effectively becoming theirs for a period of time. This implementation of Malloc ensures that any other time the program calls `malloc`, it does not return a memory address that is unavailable.
1. Free in C accepts the memory address that the programmer want to free and marks that memory address along with all the associated blocks as free. So, the next time `malloc` is called, the freed address and its associated blocks can be safely used.

## Memory Limits For a 32-bit Kernel
1. Whilst in Protected Mode, the processor is in a 32-bit state.
1. In a 32-bit state, a programmer can only access 32-bit addresses: meaning that the max amount of addressable RAM is 4.29GB regardless of the amount physically installed.
1. An array of uninitialized memory is available for addressing starting from address `0x01000000`.
1. Note that the address `0xC0000000` is reserved: meaning that the memory available starting at the address `0x01000000` grants a maximum of 3.22GB RAM.

## Motivation For Using Heap
1. Can be pointed at an address unused by hardware that is also big enough for programmers to use.
1. The Heap data size can be defined by a fixed size---like 100MB for example.
1. As long as there exists 100MB of usable memory available, the Heap will work fine.
1. The Heap will be responsible for storing information in the kernel.
1. The chosen Heap implementation will be responsible for managing the big chungus of memory we call the Heap.

## Simplest Possible Heap Implementation
1. Start with a start address & call it a *current address*, point it to somewhere free---i.e.; `0x01000000`.
1. Any call to `malloc` gets the current address, stores it in a temporary variable called `tmp`.
1. Now, the current address is incremented by the size provided to `malloc`.
1. `tmp` is returned.
1. Current address now contains the next address for `malloc` to return when `malloc` is called again.
### Benefits 
Ease of Implementation.
### Negatives
Memory can never be released; which may eventually lead to the system being unusable and requiring a reset.
**leads to mem leaks and eventually a system crash**

## Chosen Heap Implementation
1. Consists of a giant table which describes a giant piece of free memory in the system. This table will describe which memory is taken, which memory is free and so on. **It shall be called the *Entry Table***
1. Will have a pointer to a giant piece of free memory, this will be the actual heap data itself that users of `malloc` can use. **It shall be called *Data Pool*.** If the Heap can allocate 100MB of RAM then the *Data Pool* will be 100MB in size.
1. This Heap implementation will be **block-based,** each address returned from `malloc` will be aligned to 4096 and will at least be 4096 in size.
1. If the programmer requests to have "50" bytes, 4096 bytes of memory will be returned to them.

### Entry Table
1. Composed of an array of 1 byte values that represent an entry in the Heap Data Pool.
1. Array size is calculated by taking the Heap Data Pool size and dividing it by the block size (4096 in this implementation). Ex: For a 100MB Heap -> 100MB / 4096 = 25600 bytes in Entry Table.
1. Entry zero will represent address 0x01000000, entry one will represent address 0x01001000, entry two will represent address 0x01002000 and so on. (0x1000 == 4096 bytes).

### Entry Structure
Each entry in the table is 8-bit long, describes 4096 bytes of data in the Heap Data Pool by the following fields:
1. bit 0-3 -> ET_0, ET_1, ET_2, ET_3: these 4 bits represent the **entry type**.
1. bit 4-5 -> no used and left as 0s.
1. bit 6 -> IS_FIRST: a flag that is set if this is the first entry in our allocation.
1. bit 7 -> HAS_N: a flag that is set if the entry to the right is a part of our allocation.
1. Entry Types:
    1. `HEAP_BLOCK_TABLE_ENTRY_TAKEN` -> entry is taken and the address cannot be used.
    1. `HEAP_BLOCK_TABLE_ENTRY_FREE` -> entry is free and may be used.

### Data Pool
Simply, it is a **raw flat array of thousands or millions of bytes** in the Heap that can give memory upon requesting it. 

### Malloc Algorithm For Chosen Heap Implementation
1. Take the passed size from `malloc`.
1. Calculate how many blocks are needed to allocate this size.
1. If the user asks for 5000 bytes, allocate 2 blocks aka 8192 bytes because this implementation does not work for block size less than 4096 bytes.
1. Check the Entry Table for the first entry that has a type `HEAP_BLOCK_TABLE_ENTRY_FREE`. 
1. Since two blocks are required, make sure the **next entry is also free**. Otherwise, i.e.; the next entry is used, **discard the first block found** and look further in Entry Table until 2 consecutive free blocks are found.
1. Once found, mark deez blocks as taken.
1. Return the **absolute address** that the starting block represents: (heap_data_pool_start_address + (block_number * block_size)).

### Free Algorithm For Chosen Heap Implementation
1. Calculate the block number based on the address provided by the programmer to `free`.
1. Go through the Entry Table, starting at the block number calculated in the previous step; set each entry to `HEAP_BLOCK_TABLE_ENTRY_FREE` until the last block of the allocation is reached.
1. The number of blocks to free is known **because the current block getting freed will not have the `HAS_N` bit set in its entry byte.**

### Benefits of Chosen Heap Implementation
1. Fast to allocate blocks of memory since linked lists are not used: array elements can be simply indexed.
1. Fast to free blocks of memory due to the use of arrays.

### Negatives of Chosen Heap Implementation
1. Memory allocation is in memory blocks; meaning that misaligned sizes requested from the Heap will result in wasted lost bytes. *Using block size heaps is fast tho*
1. Memory fragmentation is possible: In order for multiple blocks to be allocated, they **must** be next to each other. Therefore, blocks in-between that are free are basically wasted. This can be solved by **Paging.**

