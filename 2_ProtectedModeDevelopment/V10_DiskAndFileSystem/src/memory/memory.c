#include "memory.h"


/*
 * this function fills a contiguous block of memory with a value 'c'
 */
void* memset(void* ptr, int c, size_t size)
{
    char* c_ptr = (char*) ptr;
    for (size_t i = 0; i < size; i++)
    {
        c_ptr[i] = (char) c;
    }
    
    return ptr;
}

int memcmp(void* s1, void* s2, int count)
{
    char* c1 = s1;
    char* c2 = s2;

    while (count-- > 0)
    {
        if (*c1++ != *c2++)
        {
            return c1[-1] < c2[-1] ? -1 : 1;
        }
    }

    return 0;
}

void* memcpy(void* src, void* dest, int len)
{
    char* src_tmp = (char*) src;
    char* dest_tmp = (char*) dest;

    while (len--)
    {
        *dest_tmp++ = *src_tmp++;
    }

    return dest;
}