#ifndef PROCESS_H
#define PROCESS_H

#include <stdint.h>
#include "../config.h"
#include "task.h"

// this struct defines how the kernel sees the process (assuming it is binary process)
struct process
{
    uint16_t id; // process id
    char filename[HEHOS_MAX_PATH];

    struct task* task; // main process task

    // limit on program mem allocations (malloc) to avoid mem leaks and keep track of deez allocations
    void* allocations[HEHOS_MAX_PROGRAM_ALLOCATIONS]; 

    // physical pointer to process memory
    void* ptr;

    // physical pointer to stack
    void* stack;

    // size of data pointed to by ptr
    uint32_t size;
};

int process_load_for_slot(const char* filename, struct process** process, int process_slot);
int process_load(const char* filename, struct process** process);

#endif