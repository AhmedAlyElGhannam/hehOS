#include "file.h"
#include "../config.h"
#include "../kernel.h"
#include "../memory/memory.h"
#include "../memory/heap/kheap.h"
#include "../status.h"
#include "pparser.h"
#include "../disk/disk.h"
#include "../string/string.h"

struct filesystem* filesystems[HEHOS_MAX_FILESYSTEMS]; // all filesystems 
struct file_descriptor* file_descriptors[HEHOS_MAX_FILE_DESCRIPTORS]; // all open files (fd start at 1)

static struct filesystem** fs_get_free_filesystem(void)
{
    int i = 0;
    for (i = 0; i < HEHOS_MAX_FILESYSTEMS; i++)
    { 
        if (filesystems[i] == 0)
        {
            return &filesystems[i];
        }
    }

    return 0; // no free filesystems available
}

void fs_insert_filesystem(struct filesystem* filesystem)
{
    struct filesystem** fs;
    fs = fs_get_free_filesystem();
    if (!fs)
    {
        print("Problem inserting filesystem"); 
        while(1) {}
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

void fs_init()
{
    memset(file_descriptors, 0, sizeof(file_descriptors));
    fs_load();
}

static int file_new_descriptor(struct file_descriptor** desc_out)
{
    int res = -ENOMEM; // default response
    // loop through all file descriptors
    for (int i = 0; i < HEHOS_MAX_FILE_DESCRIPTORS; i++)
    {
        if (file_descriptors[i] == 0) // aka file descriptor is free
        {
            struct file_descriptor* desc = kzalloc(sizeof(struct file_descriptor));
            desc->index = i + 1; // fd start from 1 not 0
            file_descriptors[i] = desc;
            *desc_out = desc;
            res = 0;
            break;
        }
    }

    return res;
}

static struct file_descriptor* file_get_descriptor(int fd)
{
    if (fd <= 0 || fd >= HEHOS_MAX_FILE_DESCRIPTORS)
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
    for (int i = 0; i < HEHOS_MAX_FILESYSTEMS; i++)
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

FILE_MODE file_get_mode_by_string(const char* str)
{
    FILE_MODE mode = FILE_MODE_INVALID;
    if (strncmp(str, "r", 1) == 0)
    {
        mode = FILE_MODE_READ;
    }
    else if(strncmp(str, "w", 1) == 0)
    {
        mode = FILE_MODE_WRITE;
    }
    else if(strncmp(str, "a", 1) == 0)
    {
        mode = FILE_MODE_APPEND;
    }
    return mode;
}

int fopen(const char* filename, const char* mode_str)
{
    int res = 0;
    // parse path
    struct path_root* root_path = pathparser_parse(filename, NULL);
    if (!root_path)
    {
        res = -EINVARG;
        goto out;
    }

    // if path is just the root directory sth like 0:/
    if (!root_path->first)
    {
        res = -EINVARG;
        goto out;
    }

    // ensure the disk exists before reading from it 
    struct disk* disk = disk_get(root_path->drive_no);
    if (!disk)
    {
        res = -EIO;
        goto out;
    }

    // check if there is a filesystem on this disk
    if (!disk->filesystem)
    {
        res = -EIO;
        goto out;
    }

    // get mode and check if it is valid
    FILE_MODE mode = file_get_mode_by_string(mode_str);
    if (mode == FILE_MODE_INVALID)
    {
        res = -EINVARG;
        goto out;
    }

    // get private disk data
    void* descriptor_private_data = disk->filesystem->open(disk, root_path->first, mode);
    // if (ISERR(descriptor_private_data))
    // {
    //     res = ERROR_I(descriptor_private_data);
    //     goto out;
    // }

    // create a new file descriptor
    struct file_descriptor* desc = 0;
    res = file_new_descriptor(&desc);
    if (res < 0)
    {
        goto out;
    }

    // set fs of fd as the disk's fs
    desc->filesystem = disk->filesystem;
    // set private data of fd as disk's private data
    desc->private = descriptor_private_data;
    // set fd's disk as passed disk in path
    desc->disk = disk;
    // store descriptor index here to return it
    res = desc->index;

out:
    if (res < 0)
        res = 0; // fopen should not return negative values

    return res;
}