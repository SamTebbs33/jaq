//
// Created by Samuel Tebbs on 12/04/2018.
//

#include "timer.h"
#include "../kernel/idt/idt.h"
#include "../kernel/util/util.h"
#include "../kernel/screen/print.h"

void on_tick(registers_t* registers) {
}

void timer_init(uint32_t frequency) {
    uint32_t divisor = 1193180 / frequency;

    // Send the command byte.
    outb(0x43, 0x36);

    // Divisor has to be sent byte-wise, so split here into upper/lower bytes.
    uint8_t low = (uint8_t) (divisor & 0xFF);
    uint8_t high = (uint8_t) ((divisor>>8) & 0xFF );

    // Send the frequency divisor.
    outb(0x40, low);
    outb(0x40, high);

    idt_register_irq_handler(0, on_tick);
}
