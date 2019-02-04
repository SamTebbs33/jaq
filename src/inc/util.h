//
// Created by Samuel Tebbs on 12/04/2018.
//

#ifndef JAQ_UTIL_H
#define JAQ_UTIL_H

#include <string.h>
#include <stdinc.h>
#include <stdarg.h>
#include <log.h>

#define PANIC(str, ...) { logf(LOG_LEVEL_DEBUG, "PANIC @ %s:%d: " str, __FILE__, __LINE__ __VA_OPT__(,) __VA_ARGS__); while(1); }
#define ASSERT_EQ_INT(name, val, expected) { if(val != expected) PANIC(name " assertion failed, got %d but expected %d", val, expected); }
#define ASSERT_EQ_STR(name, val, expected) { if(strcmp(val, expected) != 0) PANIC(name " assertion failed, got %s but expected %s", val, expected); }
#define ASSERT_PAGE_ALIGNED(name, val) { if(!IS_PAGE_ALIGNED(val)) PANIC(name " assertion failed, got %d", val); }
#define ASSERT_NOT_NULL(name, ptr) { if(!(ptr)) PANIC(name "assertion failed, got %d, expected not null", ptr); }

#endif //JAQ_UTIL_H
