#include "task.h"
#include "../memory/memory.h"
#include "../status.h"
#include "../config.h"
#include "../kernel.h"
#include "../memory/heap/kheap.h"
#include "process.h"
#include "../idt/idt.h"
#include "../memory/paging/paging.h"
#include "../string/string.h"
#include "../loader/formats/elfloader.h"
#include <stdint.h>

// current running task
struct task* current_task = 0;

// task linked list
struct task* task_tail = 0;
struct task* task_head = 0;

struct task* task_current(void)
{
    return current_task;
}

int task_init(struct task* task, struct process* process)
{
    memset(task, 0, sizeof(struct task));

    // map the entire 4gb address space to itself and make it accessible for all privileges
    task->page_directory = paging_new_4gb(PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL);
    if (!task->page_directory)
    {
        return -EIO;
    }

    // initialize ip to program start address
    task->registers.ip = HEHOS_PROGRAM_VIRTUAL_ADDRESS;
    if (process->filetype == PROCESS_FILE_TYPE_ELF)
    {
        task->registers.ip = elf_header(process->elf_file)->e_entry;
    }
    task->registers.ss = USER_DATA_SEGMENT;
    task->registers.cs = USER_CODE_SEGMENT;
    task->registers.esp = HEHOS_PROGRAM_VIRTUAL_STACK_ADDRESS_START;

    task->process = process;

    return 0;
}

struct task* task_get_next(void)
{
    if (!current_task->next)
    {
        return task_head;
    }

    return current_task->next;
}

static void task_list_remove(struct task* task)
{
    if (task->prev)
    {
        task->prev->next = task->next;
    }

    if (task == task_head)
    {
        task_head = task->next;
    }

    if (task == task_tail)
    {
        task_tail = task->prev;
    }

    if (task == current_task)
    {
        current_task = task_get_next();
    }

}

int task_free(struct task* task)
{
    // free paging
    paging_free_4gb(task->page_directory);
    
    // remove task from list of running tasks
    task_list_remove(task);

    // free task data
    kfree(task);

    return 0;
}

struct task* task_new(struct process* process)
{
    int res = 0;
    struct task* task = kzalloc(sizeof(struct task));
    if (!task)
    {
        res = -ENOMEM;
        goto out;
    }

    res = task_init(task, process);
    if (res != HEHOS_ALL_OK)
    {
        goto out;
    }

    if (task_head == 0)
    {
        task_head = task;
        task_tail = task;
        current_task = task;
        goto out;
    }

    // task linked list shenanigans
    task_tail->next = task;
    task->prev = task_tail;
    task_tail = task;

out:
    if (ISERR(res))
    {
        task_free(task);
        return ERROR(res);
    }

    return task;
}

int task_switch(struct task* task)
{
    current_task = task;
    paging_switch(task->page_directory);
    return 0;
}

// changes page directory from kernel's to task's
int task_page(void)
{
    user_registers();
    task_switch(current_task);
    return 0;
}

void task_run_first_ever_task(void)
{
    if (!current_task)
    {
        panic("task_run_first_ever_task: No Current Task Exists!\n");
    }

    task_switch(task_head);
    task_return(&task_head->registers);
}

void task_save_state(struct task* task, struct interrupt_frame* frame)
{
    task->registers.ip = frame->ip;
    task->registers.cs = frame->cs;
    task->registers.flags = frame->flags;
    task->registers.esp = frame->esp;
    task->registers.ss = frame->ss;
    task->registers.eax = frame->eax;
    task->registers.ebp = frame->ebp;
    task->registers.ebx = frame->ebx;
    task->registers.ecx = frame->ecx;
    task->registers.edi = frame->edi;
    task->registers.edx = frame->edx;
    task->registers.esi = frame->esi;
}

void task_current_save_state(struct interrupt_frame* frame)
{
    if (!task_current())
    {
        panic("task_current_save_state: No Current Task to Save!\n");
    }

    struct task* task = task_current();
    task_save_state(task, frame);
}

// copying is limited to a SINGLE PAGE SIZE
// for the kernel, physical and virutal addresses are mapped one-to-one
// unlike user processes, which starts at 0x400000 but can be somewhere else
// so, this function will solve this dilemma
int copy_string_from_task(struct task* task, void* virt, void* physical, int max)
{
    if (max >= PAGING_PAGE_SIZE)
    {
        return -EINVARG;
    }

    int res = 0;

    // create some memory in kernel land shared with the task's memory
    char* tmp = kzalloc(max);
    if (!tmp)
    {
        res = -ENOMEM;
        goto out;
    }

    // get table entry in task's page table for the same address 'tmp' 
    uint32_t* task_directory = task->page_directory->directory_entry;
    uint32_t old_entry = paging_get(task_directory, tmp);
    // map the physical address 'tmp' for the same virtual address in task's memory
    paging_map(task->page_directory, tmp, tmp, PAGING_IS_WRITEABLE | PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL);
    // switch to task's page table to see that memory
    paging_switch(task->page_directory);
    // can be copied safely cuz 'tmp' is the same address for both task and kernel
    strncpy(tmp, virt, max); // task is doing the copying

    // switch bacc to kernel page table   
    kernel_page();
    
    // restore the task page's old entry from before the copy operation
    res = paging_set(task_directory, tmp, old_entry);
    if (res < 0)
    {
        res = -EIO;
        goto out_free;
    }

    // copy from 'tmp' to the provided physical address 'physical'
    strncpy(physical, tmp, max);

out_free:
    kfree(tmp);

out:
    return res;
}


int task_page_task(struct task* task)
{
    user_registers();
    paging_switch(task->page_directory);
    return 0;
}

void* task_get_stack_item(struct task* task, int index)
{
    void* result = 0;

    // this value is a virtual address
    uint32_t* sp_ptr = (uint32_t*)task->registers.esp;

    // switch to the task's page in order to access this virtual address
    task_page_task(task);

    result = (void*)sp_ptr[index];

    // switch back tp kernel page
    kernel_page();

    return result;
}
