//
// Created by Samuel Tebbs on 17/04/2018.
//

#ifndef JAQ_STRING_H
#define JAQ_STRING_H

#include <stdinc.h>

void * memset(void *buff, char value, size_t len);
void* memcpy(void* dest, void* src, size_t len);

size_t strlen(const char *str);
char *strcpy(char *dest, const char *src);
char *strncpy(char *dest, const char *src, size_t len);
int strcmp(const char *s1, const char *s2);
char* strtok(char* str, char* delim);
char* strtok_r(char* str, char* delim, char** save);
int strchr(char* str, char ch);
int strrchr(char* str, char ch);
int strstr(char* str, char* substr);
char* strdup(char* str);
char* strcat(char* dest, const char* str);

#endif //JAQ_STRING_H
