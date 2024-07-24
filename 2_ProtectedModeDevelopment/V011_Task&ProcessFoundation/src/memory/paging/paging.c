#include "paging.h"
#include "../heap/kheap.h"
#include "../../status.h"
#include <stdint.h>

void paging_load_directory(uint32_t* directory);
static uint32_t* current_directory = 0;

// this function creates page directory table && page entry table where they are mapped IN A LINEAR FASHION
struct paging_4gb_chunk* paging_new_4gb(uint8_t flags)
{
    // directory is created to hold 1024 entries each of size 32-bit
    // each directory entry will point to a page table
    uint32_t* directory = kzalloc(sizeof(uint32_t) * PAGING_TOTAL_ENTRIES_PER_TABLE);
    // offset for directory entries to make sure no address spaces are repeated
    int offset = 0;
    for (int i = 0; i < PAGING_TOTAL_ENTRIES_PER_TABLE; i++)
    {
        // each entry in page directory table
        uint32_t* entry = kzalloc(sizeof(uint32_t) * PAGING_TOTAL_ENTRIES_PER_TABLE);
        for (int b = 0; b < PAGING_TOTAL_ENTRIES_PER_TABLE; b++)
        {
            // each page table entry is stored with its flags and offset
            entry[b] = (offset + (b * PAGING_PAGE_SIZE)) | flags;
        }
        // increase directory offset by 1024 new addresses in page table * size of each table entry (4096 bytes)
        offset += (PAGING_TOTAL_ENTRIES_PER_TABLE * PAGING_PAGE_SIZE);
        // update each directory entry with suitable address and flags
        directory[i] = (uint32_t)entry | flags | PAGING_IS_WRITEABLE; // pages are writable by default
    }

    // create pointer to paging chunk to hold the pointer to directory table 
    struct paging_4gb_chunk* chunk_4gb = kzalloc(sizeof(struct paging_4gb_chunk));
    chunk_4gb->directory_entry = directory;

    return chunk_4gb;
}

void paging_switch(uint32_t* directory)
{
    paging_load_directory(directory);
    current_directory = directory;
}

uint32_t* paging_4gb_chunk_get_directory(struct paging_4gb_chunk* chunk)
{
    return chunk->directory_entry;
}

bool paging_is_aligned(void* addr)
{
    return ((uint32_t)addr % PAGING_PAGE_SIZE) == 0;
} 

int paging_get_indexes(void* virtual_address, uint32_t* directory_index_out, uint32_t* table_index_out)
{
    int res = 0;
    if (!paging_is_aligned(virtual_address))
    {
        res = -EINVARG;
        goto out;
    }  

    // calculate page directory table entry index for given address
    (*directory_index_out) = ((uint32_t)virtual_address / (PAGING_TOTAL_ENTRIES_PER_TABLE * PAGING_PAGE_SIZE));
    // calculate page table entry index for given address
    (*table_index_out) = ((uint32_t) virtual_address % (PAGING_TOTAL_ENTRIES_PER_TABLE * PAGING_PAGE_SIZE) / PAGING_PAGE_SIZE);
out:
    return res;
}

int paging_set(uint32_t* directory, void* virt, uint32_t val)
{
    // check if virtual address is 4096 aligned
    if (!paging_is_aligned(virt))
    {
        return -EINVARG;
    }

    uint32_t directory_index = 0;
    uint32_t table_index = 0;

    // calculate table indices for given address
    int res = paging_get_indexes(virt, &directory_index, &table_index);
    if (res < 0)
    {
        return res;
    }

    uint32_t entry = directory[directory_index]; // directory entry variable to extract table address
    uint32_t* table = (uint32_t*)(entry & 0xFFFFF000); // address is stored in leftmost 20 bits
    table[table_index] = val; // val here is the physical address & all necessary flags for table entry

    return 0;
}

void paging_free_4gb(struct paging_4gb_chunk* chunk)
{
    for (int i = 0; i < PAGING_TOTAL_ENTRIES_PER_TABLE; i++)
    {
        uint32_t entry = chunk->directory_entry[i];
        uint32_t* table = (uint32_t*)(entry & 0xFFFFF000);
        kfree(table);
    }

    kfree(chunk->directory_entry);
    kfree(chunk);
}

void* paging_align_address(void* ptr)
{
    if (!paging_is_aligned(ptr))
    {
        return (void*)((uint32_t)ptr + PAGING_PAGE_SIZE - ((uint32_t)ptr % PAGING_PAGE_SIZE));
    }

    return ptr;
}

int paging_map(uint32_t* directory, void* virt, void* phys, int flags)
{
    // checking for alignment
    if (((unsigned int)virt % PAGING_PAGE_SIZE) || ((unsigned int)phys % PAGING_PAGE_SIZE))
    {
        return -EINVARG;
    }

    return paging_set(directory, virt, (uint32_t)phys | flags);
}

int paging_map_range(uint32_t* directory, void* virt, void* phys, int count, int flags)
{
    int res = 0;

    for (int i = 0; i < count; i++)
    {
        res = paging_map(directory, virt, phys, flags);
        if (res == 0)
        {
            break;
        }
        virt += PAGING_PAGE_SIZE;
        phys += PAGING_PAGE_SIZE;
    }

    return res;
}

int paging_map_to(uint32_t* directory, void* virt, void* phys, void* phys_end, int flags)
{
    int res = 0;

    if (((uint32_t)virt % PAGING_PAGE_SIZE != 0) || 
        ((uint32_t)phys % PAGING_PAGE_SIZE != 0) || 
        ((uint32_t)phys_end % PAGING_PAGE_SIZE != 0)) 
    {
        res = -EINVARG;
        goto out;
    }

    if ((uint32_t)phys_end < (uint32_t)phys)
    {
        res = -EINVARG;
        goto out;
    }

    uint32_t total_bytes = phys_end - phys;
    int total_pages = total_bytes / PAGING_PAGE_SIZE;
    res = paging_map_range(directory, virt, phys, total_pages, flags);

out:
    return res;
}