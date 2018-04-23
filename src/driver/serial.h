//
// Created by Samuel Tebbs on 20/04/2018.
//

#ifndef JAQ_SERIAL_H
#define JAQ_SERIAL_H

#include <stdinc.h>

#define SERIAL_BAUD_MAX 115200
#define SERIAL_BAUD_DIVISOR(baud) (uint16_t) (SERIAL_BAUD_MAX / baud)
#define SERIAL_CHAR_LENGTH 8
#define SERIAL_CHAR_LEN_TO_BITS(len) (len - 1) & 0x3
#define SERIAL_INT_MODE_ENABLED 0xFF
#define SERIAL_INT_MODE_DISABLED 0
#define SERIAL_INT_MODE(data, empty, error, status) (data & 0x1) | (empty & 0x2) | (error & 0x4) | (status & 0x8)
#define SERIAL_COM1_PORT 0x3F8

void serial_init(uint16_t port, uint32_t baud, bool enable_interrupts, uint8_t char_len, bool single_stop_bit, bool parity, uint8_t interrupts_mode);
void serial_write(uint16_t port, char ch);
void serial_write_str(uint16_t port, char* str);

#endif //JAQ_SERIAL_H
