#include "keyboard.h"
#include "../status.h"
#include "../task/process.h"
#include "../task/task.h"

static struct keyboard* keyboard_list_head = 0;
static struct keyboard* keyboard_list_last = 0;

void keyboard_init(void)
{

}

int keyboard_insert(struct keyboard* keyboard)
{
    int res = 0;
    // if this keyboard does not have an init function
    if (keyboard->init == 0)
    {
        res = -EINVARG;
        goto out;
    }

    // if a keyboard has been inserted before
    if (keyboard_list_last)
    {
        // put the new keyboard at the end of the linked list
        keyboard_list_last->next = keyboard;
        keyboard_list_last = keyboard;
    }
    else // this is the first keyboard to be inserted 
    {
        keyboard_list_head = keyboard;
        keyboard_list_last = keyboard;
    }

    res = keyboard->init(); // initialize keyboard

out:
    return res;
}

static int keyboard_get_tail_index(struct process* process)
{
    return (process->keyboard.tail_index % HEHOS_KEYBOARD_BUFFER_SIZE);
}

static int keyboard_get_head_index(struct process* process)
{
    return (process->keyboard.head_index % HEHOS_KEYBOARD_BUFFER_SIZE);
}

void keyboard_backspace(struct process* process)
{
    process->keyboard.tail_index -= 1;
    int real_index = keyboard_get_tail_index(process);
    process->keyboard.buffer[real_index] = 0x00;
}

// push operation is done by the currently running process
void keyboard_push(char c)
{
    struct process* process = process_current();
    if (!process)
    {
        return;
    }

    int real_index = keyboard_get_tail_index(process);
    process->keyboard.buffer[real_index] = c;
    process->keyboard.tail_index += 1;
}

// pop operation is done by current task
char keyboard_pop(void)
{
    if (!task_current())
    {
        return 0; // no task to pop from
    }

    struct process* process = task_current()->process;
    int real_index = keyboard_get_head_index(process);
    char c = process->keyboard.buffer[real_index];
    if (c == 0x00)
    {
        return 0; // nothing to pop 
    }

    process->keyboard.buffer[real_index] = 0x00;
    process->keyboard.head_index += 1;

    return c;
}