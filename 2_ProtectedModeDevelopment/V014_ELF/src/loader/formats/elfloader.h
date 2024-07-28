#ifndef ELF_LOADER_H
#define ELF_LOADER_H

#include "elf.h"
#include "../../config.h"

struct elf_file
{
    char filename[HEHOS_MAX_PATH];
    int in_memory_size;
    void* elf_memory; // physical memory address this elf file is loaded at
    void* virtual_base_address; // virt base addr of this binary
    void* virtual_end_address; // ending virt addr
    void* physical_base_address; // physical base addr
    void* physical_end_address; // phys end addr for this bin
};

int elf_load(const char* filename, struct elf_file** file_out);
void elf_close(struct elf_file* file);
void* elf_virtual_base(struct elf_file* file);
void* elf_virtual_base(struct elf_file* file);
void* elf_phys_base(struct elf_file* file);
void* elf_phys_end(struct elf_file* file);

#endif