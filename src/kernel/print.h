//
// Created by Samuel Tebbs on 04/04/2018.
//

#ifndef SAMIX_PRINT_H
#define SAMIX_PRINT_H

#include "framebuffer.h"

void print(char* buff, unsigned int len);
void print_bg(char bg);
void print_fg(char fg);
void print_at(unsigned int r, unsigned int c);

#define PRINT(s) print(s, sizeof(s))

#endif //SAMIX_PRINT_H
