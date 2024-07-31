#ifndef PROCESS_H
#define PROCESS_H

#include <stdint.h>
#include <stdbool.h>
#include "../config.h"
#include "task.h"

#define PROCESS_FILE_TYPE_ELF 0
#define PROCESS_FILE_TYPE_BIN 1


struct elf_file;

typedef unsigned char PROCESS_FILETYPE;

struct process_allocation
{
    void* ptr;
    size_t size;
};

struct command_argument
{
    char argument[512];
    struct command_argument* next;
};

struct process_arguments
{
    int argc;
    char** argv;
};

// this struct defines how the kernel sees the process (assuming it is binary process)
struct process
{
    uint16_t id; // process id
    char filename[HEHOS_MAX_PATH];

    struct task* task; // main process task

    struct process_arguments arguments;

    // array of program mem allocations (malloc) to avoid mem leaks and keep track of deez allocations
    struct process_allocation allocations[HEHOS_MAX_PROGRAM_ALLOCATIONS];

    // either .bin or .elf
    PROCESS_FILETYPE filetype;

    union
    {
        // physical pointer to process memory
        void* ptr;

        struct elf_file* elf_file;
    };
    

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
void* process_malloc(struct process* process, size_t size);
void process_free(struct process* process, void* ptr);
void process_get_arguments(struct process* process, int* argc, char*** argv);
int process_inject_arguments(struct process* process, struct command_argument* root_argument);
int process_terminate(struct process* process);

#endif