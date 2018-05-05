//
// Created by Samuel Tebbs on 04/04/2018.
//

#include <stdarg.h>
#include "print.h"
#include "framebuffer.h"
#include "../util/maths.h"
#include "../util/string.h"

char fg = FB_WHITE, bg = FB_BLACK;
unsigned int row = 0, column = 0;

void print_scroll(uint32_t rows) {
    fb_scroll(rows);
}

void print(char* str) {
    char ch;
    while((ch = *str++)) {
        print_ch(ch);
        if(row >= FB_ROWS) return;
    }
}

// Source: https://wiki.osdev.org/Meaty_Skeleton
void printf(const char* restrict format, ...) {
    va_list parameters;
    va_start(parameters, format);

    int written = 0;

    while (*format != '\0') {
        size_t maxrem = (size_t) (INT32_MAX - written);

        if (format[0] != '%' || format[1] == '%') {
            if (format[0] == '%')
                format++;
            size_t amount = 1;
            while (format[amount] && format[amount] != '%')
                amount++;
            if (maxrem < amount) {
                return;
            }
            print_len((char *) format, amount);
            format += amount;
            written += amount;
            continue;
        }

        const char* format_begun_at = format++;

        if (*format == 'c') {
            format++;
            char c = (char) va_arg(parameters, int /* char promotes to int */);
            if (!maxrem) {
                return;
            }
            print_len(&c, sizeof(c));
            written++;
        } else if (*format == 's') {
            format++;
            const char *str = va_arg(parameters, const char*);
            size_t len = strlen(str);
            if (maxrem < len) {
                return;
            }
            print_len((char *) str, len);
            written += len;
        } else if(*format == 'd') {
            format++;
            int i = va_arg(parameters, int);
            written += print_u32(i);
        } else {
            format = format_begun_at;
            size_t len = strlen(format);
            if (maxrem < len) {
                return;
            }
            print_len((char *) format, len);
            written += len;
            format += len;
        }
    }

    va_end(parameters);
}

void print_len(char *buff, size_t len) {
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
    row = min_u32(r, FB_ROWS);
    if(c > FB_COLUMNS) {
        column = 0;
        row = min_u32(row + 1, FB_ROWS);
    } else column = c;
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

uint32_t print_u32_rec(uint32_t u32, bool first, uint32_t count) {
    if(u32 > 0) print_u32_rec(u32 / 10, false, count + 1);
    else if(!first) return count;
    uint8_t digit = (uint8_t) (u32 % 10);
    print_ch((char) (digit + '0'));
    return count;
}

uint32_t print_u32(uint32_t u32) {
    return print_u32_rec(u32, true, 0);
}

void print_ch(char ch) {
    if(ch == PRINT_NEWLINE) print_at(row + 1, 0);
    else if(ch == PRINT_TAB) print_at(row, column + PRINT_TAB_SIZE);
    else {
        fb_putc(ch, fg, bg);
        column++;
        if (column >= FB_COLUMNS) {
            column = 0;
            row++;
        }
        fb_cursor(row, column);
    }
}
