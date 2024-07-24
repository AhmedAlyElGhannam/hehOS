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

### AmogOS Logo For... Research Purposes ( ͡° ͜ʖ ͡°)

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


. 　　　。　　　　•　 　ﾟ　　。 　　.

　　　.　　　 　　.　　　　　。　　 。　. 　

.　　 。　　　• . 　　 • 　　　　•

　　ﾟ　　 Red was not An Impostor.　 ඞ。　.

　　'　　　 1 Impostor remains 　 　　。

　　ﾟ　　　.　　　. 　　　　.　 .



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


# Paging
## Features
1. Grants the ability to **remap** memory addresses.
1. Provides the **illusion** of having the maximum amount of RAM installed.
1. Can be used to **hide** memory from other processes.

## Remapping Memory
1. Ex: memory address `0x100000` could point to address `0x200000`.
1. Paging works in blocks of size 4096 byte by default. Deez blocks are called *pages*.
1. Upon enabling paging, the Memory Management Unit (MMU) will look at the allocated page tables to resolve virtual addresses into physical addresses.
1. Paging gives the impression that memory exists even when it does not.

## Virtual Addresses vs. Physical Addresses
1. Virtual addresses are addresses that are not pointing to the address in memory that their value says they are. Virtual address `0x100000` might point to physical address `0x200000` for example.
1. Physical addresses are absolute addresses in memory whose value points to the same address in memory. For example, if physical address `0x100000` points to address `0x100000` then this is a physical address.
1. Essentially, virtual addresses and physical addresses are just terms used to explain how a piece of memory is being accessed.

## Paging Structure Overview
1. Page Directory Table -> contains 1024 page directories; **each** points to a page table.
1. Page Table -> has 1024 page entries; each maps to a physical address: covering 4096 bytes of memory.
1. Each 4096 byte block of memory is called a page.
1. PDT_entries * PT_entry * page_size = 1024 * 1024 * 4096 = 4GB of addressable memory.

### Page Directory Structure
1. Holds a pointer to a page table.
1. Holds attributes:
    1. bit 0: P (Present)-> This bit is set if the page exists in real memory. If this page is not actually available, the kernel developer should set this bit to zero. If someone accesses this page, a **page fault** will occur and it must be resolved.
    1. bit 1: R (Read/Write)-> This bit is set if the page is readable and writable. If it is not set, the page is only readable. *Note that the `WP` bit in the `CR0` register can allow writing in all cases for supervisor.*
    1. bit 2: U (User Supervisor)-> If set, the page can be accessed by all privilage ring levels. If it is not set, only supervisor ring levels can access this page.
    1. bit 3: W (Write-back/Write-through)-> If set, write-though caching is enabled. If it is not set, write-back caching is enabled instead.
    1. bit 4: D (Cache Disable)-> Set to 1 to disable page caching.
    1. bit 5: A (Page Accessed)-> Set to 1 by CPU if this page is accessed.
    1. bit 6: Left as 0---not used.
    1. bit 7: S (Page Size)-> Set to 0 for 4KB pages and to 1 for 4MB pages.
    1. bit 8: G -> Set to prevent Translation Look-aside Buffer (TLB) from updating the address in its cache if the `CR3` register is reset.
    1. bits 9-11: Available.
    1. bits 12-31: Physical 4KB-aligned Address.

### Page Table Entry Structure
    1. bit 0: P (Present)-> This bit is set if the page exists in real memory. If this page is not actually available, the kernel developer should set this bit to zero. If someone accesses this page, a **page fault** will occur and it must be resolved.
    1. bit 1: R (Read/Write)-> This bit is set if the page is readable and writable. If it is not set, the page is only readable. *Note that the `WP` bit in the `CR0` register can allow writing in all cases for supervisor.*
    1. bit 2: U (User Supervisor)-> If set, the page can be accessed by all privilage ring levels. If it is not set, only supervisor ring levels can access this page.
    1. bit 3: W (Write-back/Write-through)-> If set, write-though caching is enabled. If it is not set, write-back caching is enabled instead.
    1. bit 4: C (Cache Disable)-> Set to 1 to disable page caching.
    1. bit 5: A (Page Accessed)-> Set to 1 by CPU if this page is accessed.
    1. bit 6: D -> Indicates that the page has been written to.
    1. bit 7: Left as 0---not used.
    1. bit 8: G -> Set to prevent Translation Look-aside Buffer (TLB) from updating the address in its cache if the `CR3` register is reset.
    1. bits 9-11: Available.
    1. bits 12-31: Physical 4KB-aligned Address.

 ## Page Fault Exception
 The CPU will call the page fault interrupt `0x14` when a problem related to paging arises, such as but not limited to:
 1. If a page in memory that does not have its `P` bit set is accessed.
 1. If a page that is for supervisor is accessed by a non-supervisor.
 1. If a page is written to and it is read-only and the writer is not a supervisor.

 ## Hiding Memory From Processes
 1. If each process is given its own Page Directory Table, the memory belonging to the process can be mapped however the developer wants. It is also possible to make the process **only** able to see its memory.
 1. Hiding memory is achieved by switching the page directories when moving between processes.
 1. All processes **can access the same virtual memory addresses** but they **will point to different physical addresses**.

## Illusion of More Memory
1. It is possible to pretend that the maximum amount of memory is available even if it is not.
1. This is achieved by creating page tables that are not present Once a process accesses this non-present address, a page fault will occur. The page can then loaded back into memory without the process knowing a thing.
1. A 100MB system can act as if it has access to the full 4Gb on a 32-bit architecture.

## Benefits of Paging
1. Each process can access the same virtual memory addresses: never writing over each other.
1. Security is an added benefit as physical memory can be mapped out of a process's memory range.
1. Can be used to prevent overwriting sensitive data such as program code.

# Reading From Hard Disk
## PCI IDE Controller
1. IDE refers to the electrical specification of cables which connects ATA drives to another device.
1. IDE allows up to 4 drives to be connected:
    1. ATA (Serial): Used for modern hard drives.
    1. ATA (Parallel): Used for old hard drives.
    1. ATAPI (Serial): Used for modern optical drives.
    1. ATAPI (Paralled): Used for old optical drives.
1. Kernel programmers do not have to care it the drive is serial or parallel.

## Possible Drive Types
1. Primary Master. (Will be the main focus)
1. Primary Slave.
1. Secondary Master.
1. Secondary Slave.

## ATA Read Algorithm
1. Select master drive and pass **part of the Logical Block Address (LBA)**.
1. Send the total number of sectors to be read.
1. Send **more rest of LBA**.
1. Send read command `0x20` to the required port.
1. Poll until read operation is ready. Interrupts can be used here instead.
1. Read two bytes at a time into the buffer from the ATA controller.

# Filesystem
## What is a Filesystem?
1. A filesystem is a structure that describes how information is laid on a disk **since disks are not aware of the existence of files; only data.**
1. As a result of knowing the filesystem structure, the OS knows how to read files from the disk.

## Disks -- Revisited
1. Hard disks can be thought of as just a giant array of information split into sectors.
1. Each sector can be read into memory and is given an LBA number.
1. Files *do not exist* on the disk.
1. Disks have no concept of files.

## Filesystem Structure
1. Contains raw data for files. (the disk is not aware of this)
1. Contains the filesystem structure **header** which can explain things such as how many files are on the disk, where the root directory is located and so on.
1. The way files are laid out on disk---i.e.; **format**---is different depending on the filesystem being used: FAT16, FAT32, NTFS, and more.
1. Without filesystems, the user would be forced to read and write data through the use of sector numbers, structure would not exist, and corruption would be likely.

## FAT16 -- Components
1. Stands for File Allocation Table 16-bit.
1. The **first sector** in this filesystem format is the boot sector on a disk. Fields that describe the file system also exist in the first sector; like how many reserved sectors follow this sector.
1. The **reserved sectors** are **sectors ignored by the filesystem**. There is a field in the boot sector that specifies **how many reserved sectors there are.** Note that **the OS must ignore deez; it is not an automatic operation done by the disk: the disk has no idea and it has to be programmed into the OS.**
1. The **first file allocation table** contains values that **represent which clusters on the disk are taken and which are free.** A cluster is just a *certain number of sectors joined together to represent one cluster.*
1. The **second file allocation table** is **optional** and depends on the FAT16 header in the boot sector.
1. The **root directory** explains what files/directories are in the root directory in the filesystem. Each entry has a relative name that represents the file or directory name; attributes such as read-only, the address of the first cluster representing the data on the disk, and more.
1. The **data region** is where all the data is located.

# File Allocation Table (FAT)

## What is FAT?
1. A filesystem developed my Microsoft.
1. It consists of a series of **clusters** that holds data and a **table** that determines the state of a cluster.
1. The **boot sector** stores information about the filesystem.

## FAT16 Filesystem
1. Uses clusters to represent **data** and **subdirectories**.
1. Each cluster uses a **fixed amount of sectors** which is specified in the **boot sector**.
1. Every file in FAT16 needs to use **at least one cluster** for its data: this translates into a lot storage is wasted for small files.
1. FAT16 **cannot store files larger than 2GB** *without* larger file support. *With* large file support the maximum is **4GB**.

## FAT16 Disk Layout (Assuming Sector Size of 512 Bytes)
1. Boot Sector (aka bootloader with **33 bytes that contain info about the filesystem**) -> 512 bytes.
1. Reserved Sectors (sectors that should not be included in the file system *including the boot sector* -- **has to be at least 1**) -> fat_header.reserved_sectors * 512.
1. FAT1 (stores info that explains which part of the disk is free, inaccessible, in-use; also allows chaining clusters TOGETHAAAA) -> fat_headers.sectors_per_fat * 512.
1. FAT2 (optional -- duplicate of FAT1 and used as backup) -> fat_headers.sectors_per_fat * 512.
1. Root Directory -> fat_header.root_dir.entries * sizeof(struct fat_directory_item) **rounded to next sector if needed.**
1. Data Clusters.

## FAT16 File Allocation Table Details
1. Each entry in the table is 2 bytes long and represents a cluster in the data clusters region that is available or taken.
1. Clusters can chain together, for example a file larger than one cluster will use two clusters. The value that represents the first cluster in the file allocation table will contain the value of the next cluster. The final cluster will contain a value of `0xFFFF` signifying that there are no more clusters.
1. The cluster size is represented in the boot sector.

## FAT16 Root Directory
1. Filesystems have directories/folders. FAT16 is no different.
1. FAT16 has what is known as a root directory: the top-most directory in the system.
1. Directories contain directory entries of a fixed size.
 
# Virtual File System (VFS)

## What is VFS Layer?
1. Allows a kernel to support an **infinite** amount of filesystems.
1. Allows developers to **abstract out low-level filesystem code**.
1. Allows filesystem functionality to be **loaded or unloaded** to the kernel **at any time**.

## Unlimited Filesystems
1. Filesystem drivers can be loaded or unloaded on demand.
1. The programming interface to the filesystem remains the same for all filesystems.

## What Happens When a Disk Gets Inserted?
1. Poll each filesystem and ask if the disk holds a filesystem it can manage. (This is called **resolving the filesystem**)
1. When a filesystem that can be used with the disk is found, the disk binds itself to its implementation.

# Understanding User Land
## User Land
1. Describes a limited processor state.
1. Processes generally run in user land.
1. User land is safe because if something goes wrong the kernel is able to intervene.
1. User land is when the processor is in ring 3.
1. User land is not a special place where processes run; it is simply a term to describe the processor when it is in a privilaged limited state.

## Kernel Land
1. Kernel land is when the process is in its maximum privileged state.
1. Whilst in kernel land, any area in memory can be changed.
1. And CPU instruction can be run.
1. There is also a high risk of damage to the system if things go wrong.
1. Kernel land is when the processor is in a privileged protection ring aka ring 0.

## User Land Restrictions
1. Access to certain locations in memory can be restricted for user land processes. **(supervisor bit in page teble entry can ensure this)**
1. Access to certain CPU instructions are restricted from user land.
1. Using paging the kernel can ensure all processes cannot access each other's memory. User land code is unable to override this because it is running in an unprivileged state. The instructions for switching pages are disabled.
1. Attempting to run privileged instructions whilst in user land will cause a protection fault. The protection fault exception interrupt handler will then be responsible for solving the problem.

## Entering User Land
1. Setup **user code** and **data segments.**
1. Setup a Task Switch Segment (TSS).
1. Pretend the CPU is returning from an interrupt: **pushing flags and data to stack** before executing an `iret` instruction to change the processor's privilege state.

### Setting up The User Segment Registers
    1. GDT has to be set up using C code (in the form of a struct) instead of assembly like in the bootloader.
    1. This struct will then be passed to an assembly function that will populate the GDT.

### Task Switch Segment
1. TSS is a way for the processor to get back to kernel land when there is a system interrupt.
1. TSS explains things such as **where the kernel stack is located.**
1. Upon receiving an interrupt when the processor is in user land, the processor will switch to the kernel code and data segments. It will then restore the stack pointer located in TSS before invoking the kernel interrupt handler.
1. TSS is a struct that contains the value of **all** major cpu registers such as ESP0 and SS0: which represent kernel stack pointer and kernel stack segment respectively.

### Pretending The CPU is Returning From an Interrupt
1. Set the segment registers to the user data segment created previously---**most likely to be 0x23**. Registers such as `DS, ES, FS, GS` should be changed; but the stack segment **should not.**
1. Save the **stack pointer** in `EAX` register since the stack is about to be modified.
1. Push user data to the stack `0x23`.
1. Push the **stack pointer** saved in `EAX` earlier.
1. Push **current flags** to the stack but not before **bitwise-OR-ing the bit that re-enables interrupts**. This is important since interrupts are cleared at this moment in time and they must re-enabled to execute `iret` successfully.
1. Push user code segment---which should be `0x1B`.
1. Push the **address of the function** that needs to be run in user land.
1. Call an `iret`: forcing the processor into a user land unprivileged state.

### Getting Back to User Land When in a Kernel Interrupt
1. When an interrupt is invoked while the cpu is in user land, the cpu will **push the same registers that were pushed to get into user land in the first place.** This way, getting back to user land is very easy: upon invoking `iret` at the end of the kernel interrupt routine, the kernel will go back to the user program just after the user program's interrupt instruction.
1. In a **multi-tasking system**, user land registers will need to be salvaged when entering kernel land; which is important so that switching to the next process task may be possible. When switching back to the old task, **just swap the old registers of the task back to the reak CPU registers again** then **drop the cpu back into user land**: the task will continue executing as if nothing happened. **(can be done with timer interrupts)**

