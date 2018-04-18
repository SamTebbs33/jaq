//
// Created by Samuel Tebbs on 17/04/2018.
//

#include "string.h"

void* memset(void *buff, char value, size_t len) {
    char* buff2 = buff;
    for (size_t i = 0; i < len; i++) buff2[i] = value;
    return buff;
}

void *memcpy(void *dest, void *src, size_t len) {
    uint8_t* src2 = src, *dest2 = dest, *end = src + len;
    while (src2 != end) *dest2++ = *src2++;
    return dest2;
}

size_t strlen(const char *str) {
    const char *s = str;
    while (*s++)
        ;

    return s - str - 1;
}

// ------------------------------------------------------------------------------------------------
char *strcpy(char *dst, const char *src) {
    char c;
    char *p = dst;

    while ((c = *src++))
    {
        *p++ = c;
    }

    *p = '\0';
    return dst;
}

// ------------------------------------------------------------------------------------------------
char *strncpy(char *dst, const char *src, size_t n) {
    size_t i;

    for (i = 0 ; i < n && src[i] != '\0' ; i++)
    {
        dst[i] = src[i];
    }

    for ( ; i < n ; i++)
    {
        dst[i] = '\0';
    }

    return dst;
}

// ------------------------------------------------------------------------------------------------
int strcmp(const char *s1, const char *s2) {
    while (*s1 == *s2)
    {
        if (*s1 == '\0')
        {
            return 0;
        }

        ++s1;
        ++s2;
    }

    return *s1 - *s2;
}
