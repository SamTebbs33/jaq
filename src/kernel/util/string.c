//
// Created by Samuel Tebbs on 17/04/2018.
//

#include <util/string.h>

char* strtok_end = NULL;

void* memset(void *buff, char value, size_t len) {
    for (size_t i = 0; i < len; i++) ((char*)buff)[i] = value;
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

char* strtok(char* str, char* delim) {
    if(!str) {
        if(strtok_end) str = strtok_end;
        else return NULL;
    }

    char* str_start = str;
    char ch;
    while((ch = *str)) {
        if(strchr(delim, ch) >= 0) {
            *str = '\0';
            strtok_end = ++str;
            return str_start;
        }
    }

    return str_start;
}

int strchr(char* str, char ch) {
    char ch2;
    for (int i = 0; (ch2 = str[i]); ++i) if(ch2 == ch) return i;
    return -1;
}

int strrchr(char* str, char ch) {
    char ch2;
    size_t len = strlen(str);
    for (int i = (int) ((uint32_t)str + len - 1); (ch2 = str[i]); --i) if(ch2 == ch) return i;
    return -1;
}

