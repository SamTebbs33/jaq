//
// Created by sam on 23/04/18.
//

#include "log.h"
#include "../../driver/serial.h"

void log(char *level, char *msg) {
    serial_write_str(SERIAL_COM1_PORT, "[");
    serial_write_str(SERIAL_COM1_PORT, level);
    serial_write_str(SERIAL_COM1_PORT, "] ");
    serial_write_str(SERIAL_COM1_PORT, msg);
}

void log_info(char *msg) {
    log(LOG_LEVEL_INFO, msg);
}

void log_error(char *msg) {
    log(LOG_LEVEL_ERR, msg);
}

void log_warning(char *msg) {
    log(LOG_LEVEL_WARNING, msg);
}
