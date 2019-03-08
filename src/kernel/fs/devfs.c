//
// Created by sam on 10/02/19.
//

#include <fs/devfs.h>
#include <fs/fs.h>
#include <screen/framebuffer.h>
#include <lib/maths.h>
#include <lib/string.h>
#include <driver/serial.h>
#include <driver/keyboard.h>
#include <log/log.h>

#define PRINT_ESCAPE_PREFIX "\\[\\033["
#define PRINT_ESCAPE_SUFFIX "\\]"

fs_node_t* dev, * stdout, * stderr, * serial, * stdin;
unsigned int row = 0, column = 0;
unsigned char fg = FB_WHITE, bg = FB_BLACK;

void print_at(unsigned int r, unsigned int c) {
    row = min_u32(r, FB_ROWS);
    if(c > FB_COLUMNS) {
        column = 0;
        row = min_u32(row + 1, FB_ROWS);
    } else column = c;
    fb_cursor(r, c);
}

size_t stdout_write(fs_node_t* node, void* buff, size_t len, size_t offset) {
    char* str = (char*)buff;
    for (size_t i = 0; i < len; ++i) {
        if(strstr((char*)str + i, PRINT_ESCAPE_PREFIX) == 0) {
            i += sizeof(PRINT_ESCAPE_PREFIX) - 1;
            int end = strstr((char*)str + i, PRINT_ESCAPE_SUFFIX);
            if(end >= 0) {
                char* sequence = strndup((char*)str + i, end);
                i += sizeof(PRINT_ESCAPE_SUFFIX) - 1 + end;
                char* tok = strtok(sequence, ";");
                char* next = strtok(NULL, ";");
                bool secondary = false;
                if(next) {
                    if(strcmp(tok, "1") == 0) secondary = true;
                    tok = next;
                }
                if(strcmp(tok, "30m") == 0) fg = secondary ? FB_DARK_GREY : FB_BLACK;
                else if(strcmp(tok, "34m") == 0) fg = secondary ? FB_BLUE : FB_LIGHT_BLUE;
                else if(strcmp(tok, "32m") == 0) fg = secondary ? FB_GREEN : FB_LIGHT_GREEN;
                else if(strcmp(tok, "36m") == 0) fg = secondary ? FB_CYAN : FB_LIGHT_CYAN;
                else if(strcmp(tok, "31m") == 0) fg = secondary ? FB_RED : FB_LIGHT_RED;
                else if(strcmp(tok, "35m") == 0) fg = secondary ? FB_MAGENTA : FB_LIGHT_MAGENTA;
                else if(strcmp(tok, "33m") == 0) fg = secondary ? FB_BROWN : FB_LIGHT_BROWN;
                else if(strcmp(tok, "37m") == 0) fg = secondary ? FB_LIGHT_GREY : FB_WHITE;
                else if(strcmp(tok, "44m") == 0) bg = secondary ? FB_BLUE : FB_LIGHT_BLUE;
                else if(strcmp(tok, "42m") == 0) bg = secondary ? FB_GREEN : FB_LIGHT_GREEN;
                else if(strcmp(tok, "46m") == 0) bg = secondary ? FB_CYAN : FB_LIGHT_CYAN;
                else if(strcmp(tok, "41m") == 0) bg = secondary ? FB_RED : FB_LIGHT_RED;
                else if(strcmp(tok, "45m") == 0) bg = secondary ? FB_MAGENTA : FB_LIGHT_MAGENTA;
                else if(strcmp(tok, "43m") == 0) bg = secondary ? FB_BROWN : FB_LIGHT_BROWN;
                else if(strcmp(tok, "47m") == 0) bg = secondary ? FB_LIGHT_GREY : FB_WHITE;
            } else i -= sizeof(PRINT_ESCAPE_PREFIX) - 1; // Reset str
        }
        char ch = str[i];

        if (ch == PRINT_NEWLINE) print_at(row + 1, 0);
        else if (ch == PRINT_TAB) print_at(row, column + PRINT_TAB_SIZE);
        else {
            fb_putc(ch, fg, bg);
            print_at(row, column+1);
        }
    }
    return len;
}

size_t stdin_read(fs_node_t* node, void* buff, size_t len, size_t offset) {
    return keyboard_read_buffer(buff, len, offset);
}

size_t dev_serial_write(fs_node_t* node, void* buff, size_t len, size_t offset) {
    serial_write_len(SERIAL_COM1_PORT, (char*)buff, len);
    return len;
}

void devfs_init() {
    dev = fs_make_dir_node("dev", 0, FS_FLAGS_DIR, 0, 0, NULL, NULL, NULL, NULL, 0, 0);
    fs_create_file(dev, "/");

    stdout = fs_make_file_node("stdout", 0, 0, 0, 0, NULL, stdout_write, NULL, NULL, 0, 0);
    fb_clear(fg, bg);
    fb_cursor(0, 0);
    fs_create_file(stdout, "/dev");

    serial = fs_make_file_node("serial", 0, 0, 0, 0, NULL, dev_serial_write, NULL, NULL, 0, 0);
    fs_create_file(serial, "/dev");
    stderr = serial;

    stdin = fs_make_file_node("stdin", 0, 0, 0, 0, stdin_read, NULL, NULL, NULL, 0, 0);
}

