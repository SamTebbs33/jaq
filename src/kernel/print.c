//
// Created by Samuel Tebbs on 04/04/2018.
//

#include "print.h"
#include "framebuffer.h"
#include "stdbool.h"

char fg = FB_WHITE, bg = FB_BLACK;
unsigned int row = 0, column = 0;

void print(char* str) {
    char ch;
    while((ch = *str++)) {
        if(row >= FB_ROWS) return;
        print_ch(ch);
    }
}

void print_len(char *buff, unsigned int len) {
    char ch;
    for (unsigned int i = 0; i < len && (ch = buff[i]); ++i) {
        // Return if we're at the end of the screen
        if(row >= FB_ROWS) return;
        print_ch(ch);
    }
}

void print_bg(char arg) {
    bg = arg;
}

void print_fg(char arg) {
    fg = arg;
}

void print_at(unsigned int r, unsigned int c) {
    row = r;
    column = c;
    fb_cursor(r, c);
}

void print_clear() {
    fb_clear(fg, bg);
    fb_cursor(0, 0);
}

void print_u8(uint8_t u8) {
    print_u32(u8);
}

void print_u16(uint16_t u16) {
    print_u32(u16);
}

void print_u32_rec(uint32_t u32, bool first) {
    if(u32 > 0) print_u32_rec(u32 / 10, FALSE);
    else if(!first) return;
    uint8_t digit = (uint8_t) (u32 % 10);
    print_ch((char) (digit + '0'));
}

void print_u32(uint32_t u32) {
    print_u32_rec(u32, TRUE);
}

void print_ch(char ch) {
    fb_putc(ch, fg, bg);
    column++;
    if(column >= FB_COLUMNS) {
        column = 0;
        row++;
    }
    fb_cursor(row, column);
}
