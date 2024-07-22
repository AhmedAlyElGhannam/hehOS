#ifndef MEMORY_H
#define MEMORY_H
#include <stddef.h>
#include <stdint.h>

void* memset(void* ptr, int c, size_t size);
int memcmp(void* s1, void* s2, int count);
void* memcpy(void* src, void* dest, int len);

#endif