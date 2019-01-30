//
// Created by Samuel Tebbs on 20/04/2018.
//

#include <serial.h>
#include <util.h>
#include <arch.h>

#define SERIAL_PORT_LCR (uint16_t) 3
// Not that for stop bits sending 0 = 1 and sending 1 = 2 (or 1.5 if char bits is 5)
#define SERIAL_LCR_VAL(char_len, stop_bits, parity, msb) (uint8_t) (((char_len - 1) & 0x3) | (stop_bits << 2) | (parity & 0x7 << 3) | ((msb & 0x1) << 7))

void serial_init(uint16_t port, uint32_t baud, bool enable_interrupts, uint8_t char_len, bool single_stop_bit, bool parity, uint8_t interrupts_mode) {
    // Set MSB of LCR
    arch_outb(port + SERIAL_PORT_LCR, SERIAL_LCR_VAL(0, 0, 0, 1));
    uint16_t divisor = SERIAL_BAUD_DIVISOR(baud);
    // Send LSB of baud rate divisor divisor
    arch_outb(port, (uint8_t) (divisor & 0x00FF));
    // Send MSB of baud rate divisor divisor
    arch_outb((uint16_t) (port + 1), (uint8_t) (divisor & 0xFF00));
    // Send parameters
    arch_outb(port + SERIAL_PORT_LCR, SERIAL_LCR_VAL(char_len, single_stop_bit ? 0 : 1, parity ? 1 : 0, 0));
    // Enable interrupts and set mode, or disable them
    if(enable_interrupts) arch_outb((uint16_t) (port + 1), interrupts_mode);
    else arch_outb((uint16_t) (port + 1), 0);
}

int is_transmit_empty(uint16_t port) {
    return arch_inb((uint16_t) (port + 5)) & 0x20;
}

void serial_write(uint16_t port, char ch) {
    while (is_transmit_empty(port) == 0);
    arch_outb(port, ch);
}

void serial_write_str(uint16_t port, char *str) {
    char ch;
    while ((ch = *str++)) serial_write(port, ch);
}

void serial_write_len(uint16_t port, char *str, size_t len) {
    size_t i = 0;
    while (i++ < len) serial_write(port, *str++);
}
