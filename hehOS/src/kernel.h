#ifndef KERNEL_H
#define KERNEL_H

#define HEHOS_MAX_PATH 108

void kernel_main(void);
void panic(const char* msg);
void kernel_page(void);

extern void kernel_registers(void);

#define ERROR(value)    (void*)(value)
#define ERROR_I(value)  (int)(value)
#define ISERR(value)    ((int)value < 0)

#endif