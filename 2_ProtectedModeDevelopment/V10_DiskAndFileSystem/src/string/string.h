#ifndef STRING_H
#define STRING_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

size_t strlen(const char* str);
size_t strnlen(const char* str, int max);
char* strcpy(char* dest, const char* src);
bool is_digit(char c);
int char_to_numeric_digit(char c);
int strncmp(const char* str1, const char* str2, int n);
int istrncmp(const char* s1, const char* s2, int n);
int strnlen_terminator(const char* str, int max, char terminator);
char tolower(char s1);

#endif