#include "dstreamer.h"
#include "../memory/heap/kheap.h"
#include "disk.h"
#include "../config.h"
#include "../status.h"

#define RECURSION_MAX_CALL_PROTECC  10
static int recursion_call_count = 0;

struct disk_stream* diskstreamer_new(int disk_id)
{
    struct disk* disk = disk_get(disk_id); // select disk with passed id
    if (!disk)
    {
        return 0;
    }

    // allocate memory for disk streamer
    struct disk_stream* streamer = kzalloc(sizeof(struct disk_stream));
    streamer->pos = 0; // initialize streamer position
    streamer->disk = disk; // set disk
    return streamer;
}

int diskstreamer_seek(struct disk_stream* stream, int pos)
{
    stream->pos = pos;
    return 0;
}

int diskstreamer_read(struct disk_stream* stream, void* out, int total)
{
    // protection for max recursion calls
    if (recursion_call_count == RECURSION_MAX_CALL_PROTECC)
    {
        return -ERECMAX;
    }

    int sector = stream->pos / HEHOS_SECTOR_SIZE; // calculate sector
    int offset = stream->pos % HEHOS_SECTOR_SIZE; // calculate offset
    char buff[HEHOS_SECTOR_SIZE];

    // read one sector into buffer
    int res = disk_read_block(stream->disk, sector, 1, buff);
    if (res < 0)
    {
        goto out;
    }

    // if bytes-to-read were more than or equal a sector then READ A SECTOR
    // if bytes-to-read were less than a sector then just read them
    int total_to_read = total > HEHOS_SECTOR_SIZE ? HEHOS_SECTOR_SIZE : total;
    for (int i = 0; i < total_to_read; i++)
    {
        *(char*)out++ = buff[offset + i];
    }

    // adjust the disk stream to know current position
    stream->pos += total_to_read;
    // if there is more to read
    if (total > HEHOS_SECTOR_SIZE)
    {
        recursion_call_count += 1;
        // read the data in the next sector(s) via recursion *PANIIIIK*
        res = diskstreamer_read(stream, out, (total - HEHOS_SECTOR_SIZE));
    }

out:
    recursion_call_count = 0;
    return res;
}

void diskstreamer_close(struct disk_stream* stream)
{
    kfree(stream);
}