#include "io.h"
#include "../idt/idt.h"
#include "../task/task.h"
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
