#ifndef KEYBOARD_H
#define KEYBOARD_H

struct process;

typedef int(*KEYBOARD_INIT_FUNCTION)(void);
struct keyboard
{
    KEYBOARD_INIT_FUNCTION init;
    char name[20];
    struct keyboard* next;
};

void keyboard_init(void);
void keyboard_backspace(struct process* process);
void keyboard_push(char c);
char keyboard_pop(void);
int keyboard_insert(struct keyboard* keyboard);

#endif