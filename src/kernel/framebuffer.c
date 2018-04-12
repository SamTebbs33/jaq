#include "framebuffer.h"
#include "util.h"

#define CURSOR_CMD_PORT 0x3D4
#define CURSOR_DATA_PORT 0x3D5
#define CURSOR_CMD_HIGH_BYTE 14
#define CURSOR_CMD_LOW_BYTE 15

unsigned char* framebuffer = (unsigned char*) 0x000B8000;
unsigned int cell_counter = 0;

unsigned char make_colour(unsigned char fg, unsigned char bg) {
    return bg << 4 | fg;
}

void fb_putc(unsigned char ch, unsigned char fg, unsigned char bg) {
    if(cell_counter < FB_CELLS * 2) {
        framebuffer[cell_counter++] = ch;
        framebuffer[cell_counter++] = make_colour(fg, bg);
    }
}

void fb_cursor(unsigned int row, unsigned int column) {
    unsigned int cursor = row * FB_COLUMNS + column;
    cell_counter = cursor * 2;
    outb(CURSOR_CMD_PORT, CURSOR_CMD_HIGH_BYTE);
    outb(CURSOR_DATA_PORT, (cursor >> 8) & 0x00FF);
    outb(CURSOR_CMD_PORT, CURSOR_CMD_LOW_BYTE);
    outb(CURSOR_DATA_PORT, cursor & 0x00FF);
}

