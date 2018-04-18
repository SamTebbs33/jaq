#include "framebuffer.h"
#include "../util/util.h"

#define CURSOR_CMD_PORT 0x3D4
#define CURSOR_DATA_PORT 0x3D5
#define CURSOR_CMD_HIGH_BYTE 14
#define CURSOR_CMD_LOW_BYTE 15

uint16_t* framebuffer = (uint16_t *) 0x000B8000;
uint32_t cell_counter = 0;

unsigned char make_colour(unsigned char fg, unsigned char bg) {
    return bg << 4 | fg;
}

void fb_putc(unsigned char ch, unsigned char fg, unsigned char bg) {
    if(cell_counter < FB_CELLS) {
        framebuffer[cell_counter++] = ch | make_colour(fg, bg) << 8;
    }
}

void fb_cursor(unsigned int row, unsigned int column) {
    unsigned int cursor = row * FB_COLUMNS + column;
    cell_counter = cursor;
    outb(CURSOR_CMD_PORT, CURSOR_CMD_HIGH_BYTE);
    outb(CURSOR_DATA_PORT, (cursor >> 8) & 0x00FF);
    outb(CURSOR_CMD_PORT, CURSOR_CMD_LOW_BYTE);
    outb(CURSOR_DATA_PORT, cursor & 0x00FF);
}

void fb_clear(uint8_t fg, uint8_t bg) {
    uint8_t colour = make_colour(fg, bg);
    uint16_t blank = ' ' | (colour << 8);

    for (int i = 0; i < FB_COLUMNS * FB_ROWS; i++)
        framebuffer[i] = blank;
}

