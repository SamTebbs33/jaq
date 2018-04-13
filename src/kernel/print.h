//
// Created by Samuel Tebbs on 04/04/2018.
//

#ifndef SAMIX_PRINT_H
#define SAMIX_PRINT_H

#include "stdint.h"

void print_ch(char ch);
void print(char* buff);
void print_len(char *buff, unsigned int len);
void print_u8(uint8_t u8);
void print_u16(uint16_t u16);
void print_u32(uint32_t u32);
void print_bg(char bg);
void print_fg(char fg);
void print_at(unsigned int r, unsigned int c);
void print_clear();

#define PRINT(s) print_len(s, sizeof(s))

#endif //SAMIX_PRINT_H
