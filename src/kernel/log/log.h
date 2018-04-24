//
// Created by sam on 23/04/18.
//

#ifndef JAQ_LOG_H
#define JAQ_LOG_H

#define LOG_LEVEL_INFO "INFO"
#define LOG_LEVEL_ERR "ERROR"
#define LOG_LEVEL_WARNING "WARNING"

void log(char* level, char* msg);
void log_info(char* msg);
void log_error(char* msg);
void log_warning(char* msg);

#endif //JAQ_LOG_H
