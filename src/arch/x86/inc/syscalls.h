//
// Created by sam on 05/02/19.
//

#ifndef JAQ_SYSCALLS_H
#define JAQ_SYSCALLS_H

#include <arch_defs.h>

#define SYSCALL_NUM_HANDLERS 128

typedef arch_interrupt_handler_t syscall_handler_t;

typedef uint32_t syscall_t;

void syscalls_init();

void syscalls_register_handler(syscall_t syscall, syscall_handler_t handler);

#endif //JAQ_SYSCALLS_H
