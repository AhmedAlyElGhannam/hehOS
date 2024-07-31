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
3. Add more eastereggs **wink wink**.



https://github.com/user-attachments/assets/8207655c-5868-4fe1-8dfd-89e263e1b111

