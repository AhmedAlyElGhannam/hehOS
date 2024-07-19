#include "string.h"

size_t strlen(const char* str)
{
    size_t len = 0;

    while(str[len])
    {
        len++;
    }

    return len;
}

size_t strnlen(const char* str, int max)
{
    size_t i = 0;

    for (i = 0; i < max; i++)
    {
        if (str[i] == 0)
        {
            break;
        }
    }

    return i;
}

bool is_digit(char c)
{
    return ((c >= 48) && (c <= 57));
}

int char_to_numeric_digit(char c)
{
    return (c - 48);
}