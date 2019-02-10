//
// Created by sam on 10/02/19.
//

#include <fs/devfs.h>
#include <fs/fs.h>
#include <screen/framebuffer.h>
#include <util/maths.h>
#include <driver/serial.h>
#include <driver/keyboard.h>

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
    for (size_t i = 0; i < len; ++i) {
        char ch = ((char *) buff)[i];
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

