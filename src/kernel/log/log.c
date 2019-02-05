//
// Created by sam on 23/04/18.
//

#include <stdarg.h>
#include <stdinc.h>
#include <log/log.h>
#include <driver/serial.h>
#include <util/string.h>

void print_level(char* level) {
    serial_write_str(SERIAL_COM1_PORT, "[");
    serial_write_str(SERIAL_COM1_PORT, level);
    serial_write_str(SERIAL_COM1_PORT, "] ");
}

void log(char *level, char *msg) {
    print_level(level);
    serial_write_str(SERIAL_COM1_PORT, msg);
}

void log_info(char *msg) {
    log(LOG_LEVEL_INFO, msg);
}

void log_error(char *msg) {
    log(LOG_LEVEL_ERR, msg);
}

void log_warning(char *msg) {
    log(LOG_LEVEL_WARNING, msg);
}

void log_ch(char ch) {
    serial_write_len(SERIAL_COM1_PORT, &ch, sizeof(ch));
}

uint32_t logf_u32_rec(uint32_t u32, bool first, uint32_t count) {
    if(u32 > 0) count = logf_u32_rec(u32 / 10, false, count + 1);
    else if(!first) return count;
    uint8_t digit = (uint8_t) (u32 % 10);
    serial_write(SERIAL_COM1_PORT, (char) (digit + '0'));
    return count;
}

uint32_t log_u32(uint32_t u32) {
    return logf_u32_rec(u32, true, 0);
}

// Source: https://wiki.osdev.org/Meaty_Skeleton
void logf(char *level, const char* restrict format, ...) {
    va_list parameters;
    va_start(parameters, format);

    int written = 0;

    print_level(level);
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
            serial_write_len(SERIAL_COM1_PORT, (char *) format, amount);
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
            log_ch(c);
            written++;
        } else if (*format == 's') {
            format++;
            const char *str = va_arg(parameters, const char*);
            size_t len = strlen(str);
            if (maxrem < len) {
                return;
            }
            serial_write_len(SERIAL_COM1_PORT, (char *) str, len);
            written += len;
        } else if(*format == 'b') {
            format++;
            int i = va_arg(parameters, int);
            written += log_u32_base(i, 2);
        } else if(*format == 'x') {
            format++;
            int i = va_arg(parameters, int);
            written += log_u32_base(i, 16);
        } else if(*format == 'd') {
            format++;
            int i = va_arg(parameters, int);
            written += log_u32(i);
        } else {
            format = format_begun_at;
            size_t len = strlen(format);
            if (maxrem < len) {
                return;
            }
            serial_write_len(SERIAL_COM1_PORT, (char *) format, len);
            written += len;
            format += len;
        }
    }

    va_end(parameters);
}

void log_len(char *level, char *msg, size_t len) {
    print_level(level);
    size_t i = 0;
    while (i++ < len) serial_write(SERIAL_COM1_PORT, *msg++);
}

void log_debug(char *msg) {
    log(LOG_LEVEL_DEBUG, msg);
}

int log_u32_base_rec(uint32_t u32, int base, bool first, int count) {
    if(u32 > 0) log_u32_base_rec(u32 / base, base, false, count + 1);
    else if(!first) return count;
    uint8_t digit = (uint8_t) (u32 % base);
    log_ch((char) (digit <= 9 ? digit + '0' : (digit - 10) + 'A'));
    return count;
}

int log_u32_base(uint32_t u32, int base) {
    return log_u32_base_rec(u32, base, true, 0);
}
