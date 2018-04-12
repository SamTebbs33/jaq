//
// Created by Samuel Tebbs on 12/04/2018.
//

#ifndef JAQ_UTIL_H
#define JAQ_UTIL_H

#include "stdint.h"

void outb(uint16_t port, uint8_t val);
uint8_t inb(uint16_t port);
void memset(void* buff, size_t len, char value);

#endif //JAQ_UTIL_H
