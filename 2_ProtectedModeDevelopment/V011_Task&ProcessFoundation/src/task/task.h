#ifndef TASK_H
#define TASK_H

#include "../config.h"
#include "../memory/paging/paging.h"
#include "process.h"
#include <stdint.h>

struct registers
{
    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;
    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;

    uint32_t ip; // instruction pointer aka address of last executed instruction
    uint32_t cs;
    uint32_t flags;
    uint32_t esp;
    uint32_t ss;
};

struct process;

struct task
{
    // task page directory
    struct paging_4gb_chunk* page_directory;

    // registers of the task when the task is not running aka execution context
    struct registers registers;

    // process of task
    struct process* process;

    // next task in linked list
    struct task* next;

    // previous task in linked list
    struct task* prev;
};

struct task* task_new(struct process* process);
int task_free(struct task* task);
struct task* task_current(void);
struct task* task_get_next(void);

#endif