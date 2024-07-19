#include "pparser.h"
#include "../kernel.h"
#include "../string/string.h"
#include "../memory/memory.h"
#include "../status.h"
#include "../memory/heap/kheap.h"

static int pathparser_path_valid_format(const char* filename)
{
    int len = strnlen(filename, HEHOS_MAX_PATH);
    return (len >= 3 && is_digit(filename[0]) && memcmp((void*)&filename[1], ":/", 2) == 0);
}

static int pathparser_get_drive_by_path(const char** path)
{
    if (!pathparser_path_valid_format(*path))
    {
        return -EBADPATH;
    }

    int drive_no = char_to_numeric_digit(*path[0]);

    // add 3 bytes to skip drive number && ':' && '/'
    // this basically skips over drive number in path after extracting it
    *path += 3; 

    return drive_no;
}

static struct path_root* pathparser_create_root(int drive_number)
{
    // allocate memory the size of path root struct and return the 
    // address of first location
    struct path_root* path_r = kzalloc(sizeof(struct path_root));

    // set drive number in path root struct
    path_r->drive_no = drive_number;
    // path root is assumed to have no other path parts
    path_r->first = 0;

    // return the address of path root struct
    return path_r;
}

static const char* pathparser_get_path_part(const char** path)
{
    // allocate enough memory to hold path part name
    char* result_path_part = kzalloc(HEHOS_MAX_PATH);

    int i = 0;
    // as long as the path character is not '/' or null terminator
    while ((**path != '/') && (**path != 0x00))
    {
        // save this path character
        result_path_part[i] = (**path);
        (*path) += 1; // point to the next character's address
        i++; 
    }

    if ((**path) == '/')
    {
        // skip the '/' to avoid problems
        (*path) += 1;
    }

    // if the passed path is basically non-existent
    if (i == 0)
    {
        // free the allocated memory
        kfree(result_path_part);
        // reinitialize path part variable for next time
        result_path_part = 0;
    }

    return result_path_part;
}

struct path_part* pathparser_parse_path_part(struct path_part* last_part, const char** path)
{
    // extract first part of path
    const char* path_part_str = pathparser_get_path_part(path);
    // path parsing finished
    if (!path_part_str)
    {
        return 0;
    }

    // allocate memory for path part
    struct path_part* part = kzalloc(sizeof(struct path_part));
    part->part = path_part_str;
    part->next = 0x00; // set next to null since not sure if there is next

    // link the previous part of path with the new part IF it is not null
    if (last_part)
    {
        last_part->next = part;
    }

    return part;
}

void pathparser_free(struct path_root* root)
{
    struct path_part* part = root->first;
    struct path_part* next_part;

    // loop over all path parts that are not null
    while (part)
    {
        // store the address of next path part in another variable
        next_part = part->next;
        // free path part name memory
        kfree((void*)part->part);
        // free path part struct memory
        kfree(part);
        // shift to next part to free it
        part = next_part;
    } 

    // free path root struct memory
    kfree(root);
}

struct path_root* pathparser_parse(const char* path, const char* current_directory_path)
{
    int res = 0;
    const char* tmp_path = path;
    struct path_root* pathroot = 0;

    if (strlen(path) > HEHOS_MAX_PATH)
    {
        goto out;
    }

    res = pathparser_get_drive_by_path(&tmp_path);
    if (res < 0)
    {
        goto out;
    }

    pathroot = pathparser_create_root(res);
    if (!pathroot)
    {
        goto out;
    }

    struct path_part* first_part = pathparser_parse_path_part(NULL, &tmp_path);
    if (!first_part)
    {
        goto out;
    }

    pathroot->first = first_part;
    struct path_part* part = pathparser_parse_path_part(first_part, &tmp_path);
    while (part)
    {
        part = pathparser_parse_path_part(part, &tmp_path);
    }

out:
    return pathroot;
}