#include "isr80h.h"
#include "../idt/idt.h"
#include "../status.h"
#include "../config.h"
#include "../kernel.h"
#include "misc.h"
#include "io.h"

void isr80_register_commands(void)
{
    isr80h_register_command(SYSTEM_COMMAND0_SUM, isr80h_command0_sum);
    isr80h_register_command(SYSTEM_COMMAND1_PRINT, isr80h_command1_print);
}