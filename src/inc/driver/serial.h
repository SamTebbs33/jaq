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

/**
 * Initialise the serial system
 * @param port The serial port. See SERIAL_COM*_PORT
 * @param baud The baud rate
 * @param enable_interrupts Enable or disable serial interrupts
 * @param char_len Character length
 * @param single_stop_bit Use a single stop bit
 * @param parity Use a parity bit
 * @param interrupts_mode The interrupt mode. Use SERIAL_INT_MODE
 */
void serial_init(uint16_t port, uint32_t baud, bool enable_interrupts, uint8_t char_len, bool single_stop_bit, bool parity, uint8_t interrupts_mode);

/**
 * Write a character to a serial port
 * @param port The serial port. See SERIAL_COM*_PORT
 * @param ch The character to write
 */
void serial_write(uint16_t port, char ch);

/**
* Write a string to a serial port
* @param port The serial port. See SERIAL_COM*_PORT
* @param str The string to write
*/
void serial_write_str(uint16_t port, char* str);

/**
* Write a string to a serial port, up to a given length
* @param port The serial port. See SERIAL_COM*_PORT
* @param str The string to write
 * @param len The maximum number of characters to write
*/
void serial_write_len(uint16_t port, char* str, size_t len);

#endif //JAQ_SERIAL_H
