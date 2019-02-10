//
// Created by Samuel Tebbs on 12/04/2018.
//

#ifndef JAQ_KEYBOARD_H
#define JAQ_KEYBOARD_H

#include <stdinc.h>

#define CHAR_ESCAPE '\e'
#define CHAR_BKSPC '\b'
#define CHAR_TAB '\t'
#define CHAR_ENTER '\n'
#define CHAR_CTRL '\a'
#define CHAR_LSHIFT '\f'
#define CHAR_RSHIFT CHAR_LSHIFT
#define CHAR_ALT '\v'
#define CHAR_SPACE ' '
#define CHAR_CAPS '\x1'
#define CHAR_UP '\x11'
#define CHAR_LEFT '\x12'
#define CHAR_RIGHT '\x13'
#define CHAR_DOWN '\x14'

#define CHAR_LSHIFT_RELEASED '\xE'
#define CHAR_RSHIFT_RELEASED CHAR_LSHIFT_RELEASED
#define CHAR_ALT_RELEASED '\x10'
//TODO: Add support for function keys

#define KEYBOARD_BUFF_SIZE 64

/**
 * Initialise the keyboard, parse the default keymap and register the IRQ handler
 */
void keyboard_init();

size_t keyboard_read_buffer(char* buff, size_t len, size_t offset);

#endif //JAQ_KEYBOARD_H
