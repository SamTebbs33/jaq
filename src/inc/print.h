//
// Created by Samuel Tebbs on 04/04/2018.
//

#ifndef SAMIX_PRINT_H
#define SAMIX_PRINT_H

#include <stdinc.h>

#define PRINT_NEWLINE '\n'
#define PRINT_TAB '\t'
#define PRINT_TAB_SIZE 4

void printf(const char* restrict format, ...);
void print_ch(char ch);
void print(char* buff);
void print_len(char *buff, size_t len);
void print_u8(uint8_t u8);
void print_u16(uint16_t u16);
uint32_t print_u32(uint32_t u32);
void print_bg(char bg);
void print_fg(char fg);
void print_at(unsigned int r, unsigned int c);
void print_clear();
void print_scroll(uint32_t rows);

#define PRINT(s) print_len(s, sizeof(s))
#define PRINT_CH(ch) print_ch(ch)

#endif //SAMIX_PRINT_H
