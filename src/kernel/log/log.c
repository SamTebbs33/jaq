//
// Created by sam on 23/04/18.
//

#include "log.h"
#include "../../driver/serial.h"

void log(const char *level, char *msg) {
    serial_write_str(SERIAL_COM1_PORT, "[" LOG_LEVEL_INFO "] ");
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
