//
// Created by Samuel Tebbs on 12/04/2018.
//

#include "keyboard.h"
#include "interrupts.h"
#include "../kernel/screen/print.h"
#include "../kernel/util/util.h"

#define KEYBOARD_PORT 0x60

void on_key(interrupt_registers_t registers) {
    PRINT("key");
    uint8_t scan_code = inb(KEYBOARD_PORT);
}

void keyboard_init() {
    interrupts_register_handler(IRQ_1, on_key);
}
