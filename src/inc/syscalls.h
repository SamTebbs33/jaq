//
// Created by sam on 28/04/18.
//

#ifndef JAQ_SYS_CALLS_H
#define JAQ_SYS_CALLS_H

#include <interrupts.h>

#define SYSCALL_INTERRUPT 128
#define SYSCALL_MAX 255

typedef void (*syscall_handler_t)(interrupt_registers_t registers);
void syscall_register_handler(uint8_t syscall, syscall_handler_t handler);

#endif //JAQ_SYS_CALLS_H
