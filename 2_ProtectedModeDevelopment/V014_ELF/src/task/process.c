#include "process.h"
#include "../config.h"
#include "../memory/memory.h"
#include "../status.h"
#include "task.h"
#include "../memory/heap/kheap.h"
#include "../fs/file.h"
#include "../string/string.h"
#include "../kernel.h"
#include "../memory/paging/paging.h"
#include "../loader/formats/elfloader.h"

// current running process
struct process* current_process = 0;

static struct process* processes[HEHOS_MAX_PROCESSES] = {};

static void process_init(struct process* process)
{
    memset(process, 0, sizeof(struct process));
}

struct process* process_current(void)
{
    
    return current_process;
}

struct process* process_get(int process_id)
{
    if (process_id < 0 || process_id >= HEHOS_MAX_PROCESSES)
    {
        return NULL;
    }

    return processes[process_id];
}

// this assumes all processes share the same video memory
int process_switch(struct process* process)
{
    current_process = process;
    return 0;
}

static int process_load_binary(const char* filename, struct process* process)
{
    int res = 0;

    // open program file
    int fd = fopen(filename, "r");
    if (!fd)
    {
        res = -EIO;
        goto out;
    }

    // determine program size
    struct file_stat stat;
    res = fstat(fd, &stat);
    if (res != HEHOS_ALL_OK)
    {
        goto out;
    }

    // allocate memory for program
    void* program_data_ptr = kzalloc(stat.filesize);
    if (!program_data_ptr)
    {
        res = -ENOMEM;
        goto out;
    }

    // read program data
    if (fread(program_data_ptr, stat.filesize, 1, fd) != 1)
    {
        res = -EIO;
        goto out;
    }

    // set filetype to .bin
    process->filetype = PROCESS_FILE_TYPE_BIN;
    // set physical pointer to process mem
    process->ptr = program_data_ptr;
    // set program size
    process->size = stat.filesize;

out:
    fclose(fd);
    return res;
}

static int process_load_elf(const char* filename, struct process* process)
{
    int res = 0;
    struct elf_file* elf_file = 0;
    res = elf_load(filename, &elf_file);
    if (ISERR(res))
    {
        goto out;
    }

    process->filetype = PROCESS_FILE_TYPE_ELF;
    process->elf_file = elf_file;
out:
    return res;
}

static int process_load_data(const char* filename, struct process* process)
{
    int res = 0;
    res = process_load_elf(filename, process);
    if (res == -EINFORMAT)
    {
        res = process_load_binary(filename, process);
    }
    

    return res;
}

int process_map_binary(struct process* process)
{
    int res = 0;

    paging_map_to(process->task->page_directory, (void*)HEHOS_PROGRAM_VIRTUAL_ADDRESS, process->ptr, paging_align_address(process->ptr + process->size), PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL | PAGING_IS_WRITEABLE);

    return res;
}

static int process_map_elf(struct process* process)
{
    int res = 0;

    struct elf_file* elf_file = process->elf_file;
    res = paging_map_to(process->task->page_directory, paging_align_to_lower_page(elf_virtual_base(elf_file)), elf_phys_base(elf_file), paging_align_address(elf_phys_end(elf_file)), PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL | PAGING_IS_WRITEABLE);
    return res;
}

int process_map_memory(struct process* process)
{
    int res = 0;
    switch(process->filetype)
    {
        case PROCESS_FILE_TYPE_ELF:
            res = process_map_elf(process);
        break;

        case PROCESS_FILE_TYPE_BIN:
            res = process_map_binary(process);
        break;

        default:
            panic("process_map_memory: Invalid filetype\n");
    }

    if (ISERR(res))
    {
        goto out;
    }

    // stack mapping is from the END to START since it grows downwards
    paging_map_to(process->task->page_directory, (void*)HEHOS_PROGRAM_VIRTUAL_STACK_ADDRESS_END, process->stack, paging_align_address(process->stack+HEHOS_USER_PROGRAM_STACK_SIZE), PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL | PAGING_IS_WRITEABLE);
out:
    return res;
}

int process_load_for_slot(const char* filename, struct process** process, int process_slot)
{
    int res = 0;
    struct task* task = 0;
    struct process* _process;
    void* prog_stack_prt = 0;

    if (process_get(process_slot) != 0)
    {
        res = -EISTKN;
        goto out;
    }

    _process = kzalloc(sizeof(struct process));
    if (!_process)
    {
        res = -ENOMEM;
        goto out;
    }

    process_init(_process);
    res = process_load_data(filename, _process);
    if (res < 0)
    {
        goto out;
    }

    // initialize process stack pointer
    prog_stack_prt = kzalloc(HEHOS_USER_PROGRAM_STACK_SIZE);
    if (!prog_stack_prt)
    {
        res = -ENOMEM;
        goto out;
    }
    
    // set process filename, stack pointerm, and id 
    strncpy(_process->filename, filename, sizeof(_process->filename));
    _process->stack = prog_stack_prt;
    _process->id = process_slot;

    // create task
    task = task_new(_process);
    if (ERROR_I(task) == 0)
    {
        res = ERROR_I(task);
        goto out;
    }

    _process->task = task;

    res = process_map_memory(_process);
    if (res < 0)
    {
        goto out;
    }

    // setting process data to passed address
    *process = _process;

    // add process to array
    processes[process_slot] = _process;

out: 
    if (ISERR(res))
    {
        if (_process && _process->task)
        {
            task_free(_process->task);
        }

        // TODO: free all process data
    }

    return res;
}

int process_get_free_slot(void)
{
    for (int i = 0; i < HEHOS_MAX_PROCESSES; i++)
    {
        if (processes[i] == 0)
        {
            return i;
        }
    }

    return -EISTKN;
}

int process_load_switch(const char* filename, struct process** process)
{
    int res = process_load(filename, process);
    if (res == 0)
    {
        process_switch(*process);
    }

    return res;
}

int process_load(const char* filename, struct process** process)
{
    int res = 0;
    
    int process_slot = process_get_free_slot();
    if (process_slot < 0)
    {
        res = -EISTKN;
        goto out;
    }

    res = process_load_for_slot(filename, process, process_slot);


out:
    return res;
}