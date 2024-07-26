#include "io.h"
#include "../idt/idt.h"
#include "../task/task.h"
#include "../keyboard/keyboard.h"
#include "../kernel.h"

void* isr80h_command1_print(struct interrupt_frame* frame)
{
    void* user_space_msg_buffer = task_get_stack_item(task_current(), 0);

    // cannot print directly cuz the message is in user space
    // must copy from user space to kernel space to print SAFELY
    char buf[1024];

    copy_string_from_task(task_current(), user_space_msg_buffer, buf, sizeof(buf));

    print(buf);

    return 0;
}

void* isr80h_command2_getkey(struct interrupt_frame* frame)
{
    char c = keyboard_pop();

    // int can be casted to void* not char
    // so, simply cast to int then to void* to shut the compiler up
    return (void*)((int)c);
}

void* isr80h_command3_putchar(struct interrupt_frame* frame)
{
    // extract pushed character from stack
    char c = (char)(int)task_get_stack_item(task_current(), 0);

    // write character to terminal
    terminal_writechar(c, VGA_COLOUR_WHITE);

    return 0;
}
