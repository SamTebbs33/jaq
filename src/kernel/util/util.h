//
// Created by Samuel Tebbs on 12/04/2018.
//

#ifndef JAQ_UTIL_H
#define JAQ_UTIL_H

#include <stdinc.h>

void outb(uint16_t port, uint8_t val);
uint8_t inb(uint16_t port);

extern void log_error(char*);
extern void log_u32(uint32_t);

#define PANIC(str) { log_error("PANIC: " str " @ " __FILE__ ":"); log_u32(__LINE__); while(1); }

#endif //JAQ_UTIL_H
