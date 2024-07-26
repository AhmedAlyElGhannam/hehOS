#include "streamer.h"
#include "../memory/heap/kheap.h"
#include "disk.h"
#include "../config.h"
#include "../status.h"
#include <stdbool.h>

struct disk_stream* diskstreamer_new(int disk_id)
{
    struct disk* disk = disk_get(disk_id);
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
    int sector = stream->pos / HEHOS_SECTOR_SIZE;
    int offset = stream->pos % HEHOS_SECTOR_SIZE;
    int total_to_read = total;
    bool overflow = (offset + total_to_read) >= HEHOS_SECTOR_SIZE;
    char buf[HEHOS_SECTOR_SIZE];
    if (overflow)
    {
        total_to_read -= (offset + total_to_read) - HEHOS_SECTOR_SIZE;
    }

    // read one sector into buffer
    int res = disk_read_block(stream->disk, sector, 1, buf);
    if (res < 0)
    {
        goto out;
    }

    for (int i = 0; i < total_to_read; i++)
    {
        *(char*)out++ = buf[offset+i];
    }

    // adjust the stream
    stream->pos += total_to_read;
    // if there is more to read
    if (overflow)
    {
        // read the data in the next sector(s) via recursion *PANIIIIK*
        res = diskstreamer_read(stream, out, total-total_to_read);
    }
out:
    return res;
}

void diskstreamer_close(struct disk_stream* stream)
{
    kfree(stream);
}