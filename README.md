# hehOS 

## Introduction
This is a 32-bit kernel designed for i386 architecture following the course [Developing a Multithreaded Kernel From Scratch!](https://www.udemy.com/course/developing-a-multithreaded-kernel-from-scratch/?couponCode=LETSLEARNNOWPP) presented by the brilliant instructor Daniel McCarthy. 

## Repository Content
I made specific folders for my progress in the course---they can be distinguished by their naming scheme that begins with `V#` and so on. Additionally, for every main section in the course, a README file can be found in its directory with my notes: some of it is just rephrasing the presentation slides and some are my personal explanation for certain points that need clarification.

## Compilation & Building
1. Make sure `qemu` is installed before proceeding.
2. Go to `hehOS` directory.
3. Enter `make clean` command to delete all previously-built .o files.
4. Enter `make all` command to build hehOS from source.
5. Enter `make launch` command to launch hehOS.

## For Debugging Using qemu & GDB
1. Launch `GDB`
   ```
   gdb
   ```

2. Add symbol file to debugging session (symbol file contains info that helps with debugging like function names) and start address
   ```
   add-symbol-file ./build/kernelfull.o 0x1000000
   ```

3. Tell `GDB` to connect to a remote target `qemu` for debugging the passed binary file `os.bin` along with debugging flags: `-hda` makes `qemu` use `os.bin` as a virtual hard disk to read from; `-S` tells `qemu` to start in a suspended state and not start executing unless stated by `GDB`; `-gdb stdio` enables `GDB`'s remote debugging support in `qemu` and directs `qemu` to use standard input/output (stdio) for communicating with `GDB`.
   ```
   target remote | qemu-system-i386 -hda ./bin/os.bin -S -gdb stdio
   ```

4. To place a breakpoint (since symbol file is added, gdb will recognize function names):
   ```
   break LABEL_NAME
   ```

5. To continue executing until breakpoint is reached:
   ```
   c
   ```

6. To print a variable's value (if it is a pointer for example):
   ```
   print *PTR_NAME
   ```

7. To single step in C code:
   ```
   n
   ```

8. To switch code layout from C to Assembly:
   ```
   layout asm
   ```

9. To single step in Assembly code:
    ```
    stepi
    ```
    ```
    si
    ```

10. To switch code layout back to C:
   ```
   layout previous
   ```

## Features
1. Task and process management.
2. A FAT16 filesystem.
3. Programmable system interrupts.
4. PS/2 keyboard driver.
5. Kernel/user process memory management and paging.
6. Support for `.bin` and `.elf` file execution.
7. Solid base to implement multi-tasking.
8. Interactive shell able to parse program names and arguments and launch them.
   
## Future Work 
1. Add more programs like the already-implemented echo.
2. Implement multi-tasking.
3. Add more eastereggs like the one showcased down below **wink wink**.





https://github.com/user-attachments/assets/342dcadd-1be2-4238-9a9b-0612ad89b147


