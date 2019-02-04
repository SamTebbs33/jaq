//
// Created by Samuel Tebbs on 04/04/2018.
//

#include <stdarg.h>
#include <print.h>
#include <framebuffer.h>
#include <maths.h>
#include <string.h>

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
        } else if(*format == 'b') {
            format++;
            int i = va_arg(parameters, int);
            written += print_uint_base(i, 2);
        } else if(*format == 'x') {
            format++;
            int i = va_arg(parameters, int);
            written += print_uint_base(i, 16);
        } else if(*format == 'd') {
            format++;
            int i = va_arg(parameters, int);
            written += print_uint(i);
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

int print_uint_base_rec(uint32_t u32, int base, bool first, int count) {
    if(u32 > 0) print_uint_base_rec(u32 / base, base, false, count + 1);
    else if(!first) return count;
    uint8_t digit = (uint8_t) (u32 % base);
    print_ch((char) (digit <= 9 ? digit + '0' : (digit - 10) + 'A'));
    return count;
}

int print_uint_base(uint32_t u32, int base) {
    return print_uint_base_rec(u32, base, true, 0);
}

int print_uint(uint32_t u32) {
    return print_uint_base_rec(u32, 10, true, 0);
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
