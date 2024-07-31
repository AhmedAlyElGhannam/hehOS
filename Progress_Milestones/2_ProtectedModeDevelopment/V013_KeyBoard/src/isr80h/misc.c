#include "misc.h"
#include "../idt/idt.h"
#include "../task/task.h"

void* isr80h_command0_sum(struct interrupt_frame* frame)
{
    // to understand this index thing, see this example:
    // a function has been called, stack pointer points to initial address aka index 0
    // first value was pushed, it will be stored in index 0 then sp increments
    // second value was pushed, it will be stored in index 1 then sp incements
    // sp is then REINITIALIZED aka index 0 again
    // so, v1 is accessed with index 0 && v2 is accessed with index 1
    int v2 = (int)task_get_stack_item(task_current(), 1);
    int v1 = (int)task_get_stack_item(task_current(), 0);
    
    return (void*)(v1 + v2);
}