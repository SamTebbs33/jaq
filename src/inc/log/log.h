//
// Created by sam on 23/04/18.
//

#ifndef JAQ_LOG_H
#define JAQ_LOG_H

#include <stdinc.h>

#define LOG_LEVEL_INFO "INFO"
#define LOG_LEVEL_ERR "ERROR"
#define LOG_LEVEL_WARNING "WARNING"
#define LOG_LEVEL_DEBUG "DEBUG"

/**
 * Log a string with a specified level
 * @param level The level at which to log
 * @param msg The string to log
 */
void log(char* level, char* msg);

/**
 * Log a string with a specified level, up to a maximum length
 * @param level The level at which to log
 * @param msg The string to log
 * @param len The maximum number of characters to log
 */
void log_len(char* level, char* msg, size_t len);

/**
 * Log at a level with a format and arguments
 * @param level The level to log at
 * @param format The log format. The supported specifiers are:
 *  %c: for a character
 *  %s: for a string
 *  %d: for an int
 * @param ... The arguments, which must correspond to the specifiers in the given format
 */
void logf(char* level, const char* restrict format, ...);

/**
 * Log an unsigned int with a certain base
 * @param u32 The unsigned int to log
 * @param base The base
 * @return The number of characters logged
 */
int log_u32_base(uint32_t u32, int base);

/**
 * Log at LOG_LEVEL_INFO
 * @param msg The string to log
 */
void log_info(char* msg);

/**
 * Log at LOG_LEVEL_ERROR
 * @param msg The string to log
 */
void log_error(char* msg);

/**
 * Log at LOG_LEVEL_DEBUG
 * @param msg The string to log
 */
void log_debug(char* msg);

/**
 * Log at LOG_LEVEL_WARNING
 * @param msg The string to log
 */
void log_warning(char* msg);

#endif //JAQ_LOG_H
