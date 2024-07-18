# hehOS (Formally Known as PeachOS)

## Introduction
This is a 32-bit kernel designed for i386 architecture following the course [Developing a Multithreaded Kernel From Scratch!](https://www.udemy.com/course/developing-a-multithreaded-kernel-from-scratch/?couponCode=LETSLEARNNOWPP) presented by the brilliant instructor Daniel McCarthy. 

## Repository Features
I made specific folders for my progression in the course---they can be distinguished by their naming scheme that begins with `V#` and so on. Additionally, for every main section in the course, a README file can be found in its directory with my notes: some of it is just rephrasing the presentation slides and some are my personal explanation for certain points that need clarification.

## Compilation & Building
1. Make sure `qemu` is installed before proceeding.
2. Go to `hehOS_Complete` directory.
3. Enter `make clean` command to delete all previously-built .o files.
4. Enter `make all` command to build hehOS from source.
5. Enter `make launch` command to launch hehOS.
