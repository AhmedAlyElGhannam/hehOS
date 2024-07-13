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