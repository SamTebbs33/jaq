//
// Created by Samuel Tebbs on 12/04/2018.
//

#include "util.h"

void outb(uint16_t port, uint8_t val) {
    asm volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

uint8_t inb(uint16_t port) {
    uint8_t ret;
    asm volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

void memset(void* buff, size_t len, char value) {
    char* buff2 = buff;
    for (size_t i = 0; i < len; i++) buff2[i] = value;
}

int8_t sign_i32(int32_t i) {
    return (int8_t) (i < 0 ? -1 : (i == 0 ? 0 : 1));
}
