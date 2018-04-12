//
// Created by Samuel Tebbs on 12/04/2018.
//

#include "keyboard.h"
#include "interrupts.h"
#include "print.h"

void on_key(interrupt_registers_t registers) {
    PRINT("key");
}

void keyboard_init() {
    interrupts_register_handler(IRQ_1, on_key);
}
