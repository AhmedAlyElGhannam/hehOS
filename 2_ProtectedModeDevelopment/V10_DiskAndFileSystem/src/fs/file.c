#include "file.h"
#include "../config.h"
#include "../kernel.h"
#include "../memory/memory.h"
#include "../memory/heap/kheap.h"
#include "../status.h"

struct filesystem* filesystems[hehOS_MAX_FILESYSTEMS]; // all filesystems 
struct file_descriptor* file_descriptors[hehOS_MAX_FILE_DESCRIPTORS]; // all open files (fd start at 1)

static struct filesystem** fs_get_free_filesystem(void)
{
    int i = 0;
    for (i = 0; i < hehOS_MAX_FILESYSTEMS; i++)
    {
        if (filesystems[i] == 0)
        {
            return &filesystems[i];
        }
    }

    return 0; // no free filesystems available
}

void fs_insert_filesystem(struct filesystem *filesystem)
{
    if (filesystem == 0)
    {
        panic("fs_insert_filesystem: Invalid Filesystem Pointer!\n");
    }
    
    struct filesystem** fs;
    fs = fs_get_free_filesystem();
    if (!fs)
    {
        panic("fs_insert_filesystem: Cannot Get Free Filesystem!\n");
    }

    *fs = filesystem; // add filesystem to array of filesystems
}

// loads filesystems built-in the core kernel not ones that are dynamically loaded
static void fs_static_load(void)
{
    fs_insert_filesystem(fat16_init());
}

void fs_load(void)
{
    memset(filesystems, 0, sizeof(filesystems));
    fs_static_load();
}

void fs_init(void)
{
    memset(file_descriptors, 0, sizeof(file_descriptors));
    fs_load();
}

static int file_new_descriptor(struct file_descriptor** desc_out)
{
    int res = -ENOMEM; // default response
    // loop through all file descriptors
    for (int i = 0; i < hehOS_MAX_FILE_DESCRIPTORS; i++)
    {
        if (file_descriptors[i] == 0) // aka file descriptor is free
        {
            struct file_descriptor* desc = kzalloc(sizeof(struct file_descriptor));
            desc->index = i + 1; // fd start from 1 not 0
            file_descriptors[i] = desc;
            (*desc_out) = desc;
            res = 0;
            break;
        }
    }

    return res;
}

static struct file_descriptor* file_get_descriptor(int fd)
{
    if (fd <= 0 || fd >= hehOS_MAX_FILE_DESCRIPTORS)
    {
        return 0;
    }

    int index = fd - 1; // index start at 0 & fd starts at 1

    return file_descriptors[index];
}

struct filesystem* fs_resolve(struct disk* disk)
{
    struct filesystem* fs = 0;
    
    // loop through all filesystems 
    for (int i = 0; i < hehOS_MAX_FILESYSTEMS; i++)
    {
        // if filesystem exists && can resolve the passed disk
        if (filesystems[i] != 0 && filesystems[i]->resolve(disk) == 0)
        {
            fs = filesystems[i];
            break;
        }
    }

    return fs;
} 

int fopen(const char* filename, const char* mode)
{
    return -EIO;
}
