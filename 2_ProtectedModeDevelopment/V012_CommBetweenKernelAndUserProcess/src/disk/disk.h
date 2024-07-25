#ifndef DISK_H
#define DISK_H

#include "../fs/file.h"

typedef unsigned int HEHOS_DISK_TYPE;


// this represents a real physical hard disk
#define HEHOS_DISK_TYPE_REAL 0

struct disk
{
    HEHOS_DISK_TYPE type;
    int sector_size;

    // disk id
    int id;

    struct filesystem* filesystem;

    // private data for filesystem
    void* fs_private;
};

void disk_search_and_init();
struct disk* disk_get(int index);
int disk_read_block(struct disk* idisk, unsigned int lba, int total, void* buf);

#endif