//
// Created by Samuel Tebbs on 20/04/2018.
//

#include "serial.h"
#include "../kernel/util/util.h"

#define SERIAL_PORT_LCR (uint16_t) 3
// Not that for stop bits sending 0 = 1 and sending 1 = 2 (or 1.5 if char bits is 5)
#define SERIAL_LCR_VAL(char_len, stop_bits, parity, msb) (uint8_t) ((char_len - 1) & 0x3 | stop_bits << 2 | (parity & 0x7 << 3) | (msb & 0x1) << 7)

void serial_init(uint16_t port, uint32_t baud, bool enable_interrupts, uint8_t char_len, bool single_stop_bit, bool parity, uint8_t interrupts_mode) {
    // Set baud rate
    outb(port + SERIAL_PORT_LCR, SERIAL_LCR_VAL(0, 0, 0, 1)); // Set MSBit of LCR
    uint16_t divisor = SERIAL_BAUD_DIVISOR(baud);
    outb(port, (uint8_t) (divisor & 0x00FF)); // Send LSB of divisor
    outb((uint16_t) (port + 1), (uint8_t) (divisor & 0xFF00)); // Send MSB of divisor
    outb(port + SERIAL_PORT_LCR, SERIAL_LCR_VAL(char_len, single_stop_bit ? 0 : 1, parity ? 1 : 0, 0)); // Send parameters
    // Enable interrupts and set mode, or disable them
    if(enable_interrupts) outb((uint16_t) (port + 1), interrupts_mode);
    else outb((uint16_t) (port + 1), 0);
}

int is_transmit_empty(uint16_t port) {
    return inb((uint16_t) (port + 5)) & 0x20;
}

void serial_write(uint16_t port, char ch) {
    while (is_transmit_empty(port) == 0);
    outb(port, ch);
}
