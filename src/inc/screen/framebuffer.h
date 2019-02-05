//
// Created by Samuel Tebbs on 04/04/2018.
//

#ifndef PROJECT_FRAMEBUFFER_H
#define PROJECT_FRAMEBUFFER_H

#include <stdinc.h>

#define FB_BLACK 0
#define FB_BLUE 1
#define FB_GREEN 2
#define FB_CYAN 3
#define FB_RED 4
#define FB_MAGENTA 5
#define FB_BROWN 6
#define FB_LIGHT_GREY 7
#define FB_DARK_GREY 8
#define FB_LIGHT_BLUE 9
#define FB_LIGHT_GREEN 10
#define FB_LIGHT_CYAN 11
#define FB_LIGHT_RED 12
#define FB_LIGHT_MAGENTA 13
#define FB_LIGHT_BROWN 14
#define FB_WHITE 15

#define FB_COLUMNS 80
#define FB_ROWS 25
#define FB_CELLS FB_COLUMNS * FB_ROWS

/**
 * Put a character at the cursor and advance the cursor along by one
 * @param ch The character
 * @param fg The foreground colour
 * @param bg The background colour
 */
void fb_putc(unsigned char ch, unsigned char fg, unsigned char bg);

/**
 * Put the cursor at a certain row and column
 * @param row The row at which to place the cursor
 * @param column The column at which to place the cursor
 */
void fb_cursor(unsigned int row, unsigned int column);

/**
 * Clear the screen by printing spaces. Doesn't reset the cursor
 * @param fg
 * @param bg
 */
void fb_clear(uint8_t fg, uint8_t bg);

/**
 * Scroll the framebuffer by a number of rows
 * @param rows The number of rows to scroll by
 */
void fb_scroll(uint32_t rows);

#endif //PROJECT_FRAMEBUFFER_H
