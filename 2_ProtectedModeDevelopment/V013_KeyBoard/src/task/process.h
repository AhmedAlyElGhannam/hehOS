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

    struct keyboard_buffer
    {
        char buffer[HEHOS_KEYBOARD_BUFFER_SIZE];
        int head_index;
        int tail_index;
    } keyboard;
};

int process_switch(struct process* process);
int process_load_switch(const char* filename, struct process** process);
int process_load(const char* filename, struct process** process);
int process_load_for_slot(const char* filename, struct process** process, int process_slot);
struct process* process_current(void);
struct process* process_get(int process_id);

#endif