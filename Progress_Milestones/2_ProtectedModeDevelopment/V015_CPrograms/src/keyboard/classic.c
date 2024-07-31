#include "keyboard.h"
#include "classic.h"
#include "../io/io.h"
#include "../task/task.h"
#include "../idt/idt.h"
#include "../kernel.h"
#include "../terminal/terminal.h"
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define CLASSIC_KEYBOARD_CAPSLOCK           0x3A

#define CLASSIC_KEYBOARD_CTRL_PRESSED       0x1D
#define CLASSIC_KEYBOARD_CTRL_RELEASED      0x9D

#define CLASSIC_KEYBOARD_L_ALT_PRESSED       0x38
#define CLASSIC_KEYBOARD_L_ALT_RELEASED      0xB8

static bool is_ctrl_pressed = false;
static bool is_alt_pressed = false;

int classic_keyboard_init(void);
void classic_keyboard_handle_interrupt(void);

// scan code set one for a US qwerty keyboard as defined on OSdev
// numbers are repeated cuz it accounts for numpad
static uint8_t keyboard_scan_set_one[] = {
    0x00, 0x1B, '1', '2', '3', '4', '5',
    '6', '7', '8', '9', '0', '-', '=',
    0x08, '\t', 'Q', 'W', 'E', 'R', 'T',
    'Y', 'U', 'I', 'O', 'P', '[', ']',
    0x0d, 0x00, 'A', 'S', 'D', 'F', 'G',
    'H', 'J', 'K', 'L', ';', '\'', '`', 
    0x00, '\\', 'Z', 'X', 'C', 'V', 'B',
    'N', 'M', ',', '.', '/', 0x00, '*',
    0x00, 0x20, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, '7', '8', '9', '-', '4', '5',
    '6', '+', '1', '2', '3', '0', '.'
};

// PS2 keyboard
struct keyboard classic_keyboard = {
    .name = {"Classic"},
    .init = classic_keyboard_init
};

int classic_keyboard_init(void)
{
    idt_register_interrupt_callback(ISR_KEYBRD_INT, (INTERRUPT_CALLBACK_FUNCTION)classic_keyboard_handle_interrupt);
    keyboard_set_capslock(&classic_keyboard, KEYBOARD_CAPS_LOCK_OFF);
    outb(PS2_PORT, PS2_COMMAND_ENABLE_FIRST_PORT);
    return 0;
}

uint8_t classic_keyboard_scancode_to_char(uint8_t scancode)
{
    size_t size_of_keyboard_set_one = sizeof(keyboard_scan_set_one) / sizeof(uint8_t);
    if (scancode > size_of_keyboard_set_one)
    {
        return 0;
    }

    // index scan set one with given scancode to get its ASCII value
    char c = keyboard_scan_set_one[scancode];
    if (keyboard_get_capslock(&classic_keyboard) == KEYBOARD_CAPS_LOCK_OFF)
    {
        if (c >= 'A' && c <= 'Z')
        {
            c += 32;
        }
    }

    return c;
}

void classic_keyboard_handle_interrupt(void)
{
    kernel_page();
    
    uint8_t scancode = 0;
    scancode = insb(KEYBOARD_INPUT_PORT);
    insb(KEYBOARD_INPUT_PORT);

    // will not handle key releases for now
    if (scancode & CLASSIC_KEYBOARD_KEY_RELEASED)
    {
        return;
    }

    if (scancode == CLASSIC_KEYBOARD_CAPSLOCK)
    {
        KEYBOARD_CAPS_LOCK_STATE old_state = keyboard_get_capslock(&classic_keyboard);
        keyboard_set_capslock(&classic_keyboard, old_state == KEYBOARD_CAPS_LOCK_ON ? KEYBOARD_CAPS_LOCK_OFF : KEYBOARD_CAPS_LOCK_ON);
    }

    if (scancode == CLASSIC_KEYBOARD_CTRL_PRESSED || scancode == CLASSIC_KEYBOARD_CTRL_RELEASED)
    {
        is_ctrl_pressed = true;
        return;
    }

    if (scancode == CLASSIC_KEYBOARD_L_ALT_PRESSED || scancode == CLASSIC_KEYBOARD_L_ALT_RELEASED)
    {
        is_alt_pressed = true;
        return;
    }

    uint8_t c = classic_keyboard_scancode_to_char(scancode);
    // if c is a valid character
    if (c != 0)
    {
        // ctrl + l to clear terminal
        if ((c == 'L' || c == 'l') && (is_ctrl_pressed))
        {
            is_ctrl_pressed = false;
            terminal_clear();
            return;
        }

        // ctrl + alt + s to print sussy baka
        if ((c == 'S' || c == 's') && (is_ctrl_pressed) && (is_alt_pressed))
        {
            is_ctrl_pressed = false;
            is_alt_pressed = false;
            print_sussy_bakka();
            return;
        }

        keyboard_push(c);
    }

    task_page();

}

struct keyboard* classic_init(void)
{
    return &classic_keyboard;
}

