#ifndef HEHOS_H
#define HEHOS_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

struct command_argument
{
    char argument[512];
    struct command_argument* next;
};

struct process_arguments
{
    int argc;
    char** argv;
};

extern void print(const char* filename);
extern int hehos_getkey(void);
extern void* hehos_malloc(size_t size);
extern void hehos_free(void* ptr);
extern void hehos_putchar(char c);
extern void hehos_process_get_arguments(struct process_arguments* arguments);
extern int hehos_system(struct command_argument* arguments);
extern void hehos_exit(void);

int hehos_getkeyblock(void);
void hehos_terminal_readline(char* out, int max, bool output_while_typing);
void hehos_process_load_start(const char* filename);
int hehos_system_run(const char* command);

#endif