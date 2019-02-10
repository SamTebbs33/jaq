//
// Created by Samuel Tebbs on 12/04/2018.
//

#include <driver/timer.h>
#include <arch.h>
#include <util/util.h>
#include <screen/print.h>
#include <driver/keyboard.h>
#include <fs/fs.h>
#include <fs/devfs.h>

void on_tick(arch_registers_t* registers) {
    char key_buff[KEYBOARD_BUFF_SIZE + 1];
    size_t len = fs_read(stdin, key_buff, KEYBOARD_BUFF_SIZE, 0);
    if(len > 0) {
        key_buff[len] = '\0';
        print(key_buff);
    }
}

void timer_init(uint32_t frequency) {
    uint32_t divisor = 1193180 / frequency;

    // Send the command byte.
    arch_outb(0x43, 0x36);

    // Divisor has to be sent byte-wise, so split here into upper/lower bytes.
    uint8_t low = (uint8_t) (divisor & 0xFF);
    uint8_t high = (uint8_t) ((divisor>>8) & 0xFF );

    // Send the frequency divisor.
    arch_outb(0x40, low);
    arch_outb(0x40, high);

    arch_register_interrupt_handler(ARCH_INTERRUPT_TIMER, on_tick);
}
