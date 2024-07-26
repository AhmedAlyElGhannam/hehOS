#ifndef ISR80H_H
#define ISR80H_H

enum SystemCommands
{
    SYSTEM_COMMAND0_SUM,
    SYSTEM_COMMAND1_PRINT,
};

void isr80_register_commands(void);

#endif