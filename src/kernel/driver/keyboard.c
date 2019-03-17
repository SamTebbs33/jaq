//
// Created by Samuel Tebbs on 12/04/2018.
//

#include <driver/keyboard.h>
#include <screen/print.h>
#include <arch.h>
#include <fs/fs.h>
#include <log/log.h>
#include <lib/string.h>
#include <lib/maths.h>

#define KEYBOARD_PORT 0x60

struct key_mapping {
    char ascii;
    const char* name;
};

/**
 * This maps special characters to their identifiers in a keymap file
 */
struct key_mapping mapping[] = {
        {CHAR_ESCAPE, "esc"},
        {CHAR_BKSPC, "bkspc"},
        {CHAR_TAB, "tab"},
        {CHAR_ENTER, "enter"},
        {CHAR_CTRL, "ctrl"},
        {CHAR_LSHIFT, "lshift"},
        {CHAR_RSHIFT, "rshift"},
        {CHAR_ALT, "alt"},
        {CHAR_SPACE, "space"},
        {CHAR_CAPS, "caps"},
        {CHAR_UP, "up"},
        {CHAR_LEFT, "left"},
        {CHAR_RIGHT, "right"},
        {CHAR_DOWN, "down"},
        {CHAR_ALT_RELEASED, "alt-r"},
        {CHAR_RSHIFT_RELEASED, "rshift-r"},
        {CHAR_LSHIFT_RELEASED, "lshift-r"},
        {'\0', NULL}
};

/**
 * A map between scan codes and ascii characters. The second dimension of the array determines
 * the character for each mode, with mode 0 being without shift or alt, mode 1 with shift and
 * mode 2 with alt
 */
char keymap[256][3];
volatile int modifier = 0;
volatile char key_buff[KEYBOARD_BUFF_SIZE];
volatile uint32_t key_buff_pos = 0;

void on_key(arch_cpu_state_t* registers) {
    uint8_t scan_code = arch_inb(KEYBOARD_PORT);
    char ascii = keymap[scan_code][modifier];
    switch (ascii) {
        case CHAR_ALT:
            modifier = 2;
            break;
        case CHAR_LSHIFT:
            modifier = 1;
            break;
        case CHAR_LSHIFT_RELEASED:
        case CHAR_ALT_RELEASED:
            modifier = 0;
            break;
        default:
            if(ascii >= ' ' && ascii <= '~' && key_buff_pos < KEYBOARD_BUFF_SIZE) key_buff[key_buff_pos++] = ascii;
    }
    arch_acknowledge_irq(ARCH_INTERRUPT_KEYBOARD);
}

bool is_digit(char ch) {
    return ch >= '0' && ch <= '9';
}

int to_digit(char ch) {
    return ch - '0';
}

bool parse_keymap(char* buff) {
    unsigned int i = 0;
    size_t len = strlen(buff);
    while(i < len) {
        int scan_code = 0;
        char ch;
        // Parse the scan code
        while(i < len && (ch=buff[i]) != ':' ){
			if(ch == '\n') break;
            if(!is_digit(ch)) {
                log_error("expected digit before :");
                return false;
            }
            scan_code = scan_code * 10 + to_digit(buff[i]);
            i++;
        }
        i++;
        // The type: normal, shifted or alted
        int char_type = 0;
        // Parse the ascii characters this scan code applies to
        while(i < len) {
            if(char_type >= 3) {
                log_error("Too many character types");
                return false;
            }
            char char_buff[16];
            memset(char_buff, '\0', sizeof(char_buff));
            size_t j = 0;
            // Parse a string until hitting a space or new line
            while(i < len && j < sizeof(char_buff) - 1 && (ch=buff[i]) != ' ' && ch != '\n') {
                char_buff[j] = ch;
                i++;
                j++;
            }
            char_buff[j] = '\0';
            char key_char = '\0';
            // Look through the special mappings to see if it matches one of them
            for (struct key_mapping* k = mapping; k->name; k++) {
                if(strcmp(char_buff, k->name) == 0) {
                    key_char = k->ascii;
                    break;
                }
            }
            // If it isn't a special mapping, set it to the first character of the string read
            if(key_char == '\0') key_char = char_buff[0];
            keymap[scan_code][char_type] = key_char;
            // Parse the next type (i.e. normal -> shifted, shifted -> alted)
            char_type++;
            i++;
            if (i < len && ch == '\n') {
                break;
            }
        }
    }
    return true;
}

bool load_keymap(char* path) {
    fs_node_t* file = fs_walk_path(path);
    if(file) {
        // Limit keymap files to 512 bytes, for now
        char buff[512];
        if(fs_read(file, buff, sizeof(buff), 0) > 0) return parse_keymap(buff);
    } else logf(LOG_LEVEL_ERR, "Couldn't find keymap at '%s'\n", path);
    return false;
}

void keyboard_init() {
    memset(keymap, 0, 256 * 3);
    load_keymap("/initrd/keymaps/macbook_en_GB.txt");
    arch_register_interrupt_handler(ARCH_INTERRUPT_KEYBOARD, on_key);
}

size_t keyboard_read_buffer(char *buff, size_t len, size_t offset) {
    len = min_size(len, key_buff_pos);
    memcpy(buff + offset, (void*)key_buff, len);
    memcpy((void*)key_buff, (void*)key_buff + key_buff_pos, key_buff_pos);
    key_buff_pos = 0;
    return len;
}
