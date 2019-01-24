//
// Created by Samuel Tebbs on 12/04/2018.
//

#include "keyboard.h"
#include "../kernel/idt/idt.h"
#include "../kernel/screen/print.h"
#include "../kernel/util/util.h"

#define KEYBOARD_PORT 0x60

void on_key(registers_t* registers) {
    PRINT("key");
    uint8_t scan_code = inb(KEYBOARD_PORT);
}

void keyboard_init() {
    idt_register_irq_handler(1, on_key);
}
