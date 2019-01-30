//
// Created by sam on 26/01/19.
//

#ifndef JAQ_ARCH_TYPES_H
#define JAQ_ARCH_TYPES_H

#include <paging.h>

/**
 * The x86 registers saved on interrupt
 */
struct arch_registers {
    unsigned int gs, fs, es, ds;
    unsigned int edi, esi, ebp, esp, ebx, edx, ecx, eax;
    unsigned int int_no, err_code;
    unsigned int eip, cs, eflags, useresp, ss;
};
typedef struct arch_registers arch_registers_t;

/**
 * An x86 interrupt handler
 */
typedef void (*arch_interrupt_handler_t) (arch_registers_t*);

#define ARCH_INTERRUPT_TIMER 32
#define ARCH_INTERRUPT_KEYBOARD 33
#define ARCH_PAGE_SIZE PAGE_SIZE

#endif //JAQ_ARCH_TYPES_H
