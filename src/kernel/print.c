//
// Created by Samuel Tebbs on 04/04/2018.
//

#include "print.h"
#include "framebuffer.h"

char fg = FB_WHITE, bg = FB_BLACK;
unsigned int row = 0, column = 0;

void print(char *buff, unsigned int len) {
    char ch;
    for (unsigned int i = 0; i < len && (ch = buff[i]); ++i) {
        // Return if we're at the end of the screen
        if(row >= FB_ROWS) return;
        fb_putc(ch, fg, bg);
        column++;
        if(column >= FB_COLUMNS) {
            column = 0;
            row++;
        }
    }
    fb_cursor(row, column);
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
