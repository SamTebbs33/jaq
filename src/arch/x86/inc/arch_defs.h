//
// Created by sam on 26/01/19.
//

#ifndef JAQ_ARCH_TYPES_H
#define JAQ_ARCH_TYPES_H

#include <paging.h>
#include <stdinc.h>

#define ARCH "x86"

/**
 * The x86 registers saved on interrupt. Changes to this may require changes to multitasking.s and idt_asm.s
 */
struct arch_cpu_state {
    unsigned int gs, fs, es, ds;
    unsigned int edi, esi, ebp, esp, ebx, edx, ecx, eax;
    unsigned int int_no, err_code;
    unsigned int eip, cs, eflags, useresp, ss;
};
typedef struct arch_cpu_state arch_cpu_state_t;

/**
 * An x86 interrupt handler
 */
typedef void (*arch_interrupt_handler_t) (arch_cpu_state_t*);
typedef arch_interrupt_handler_t arch_syscall_handler_t;

#define ARCH_INTERRUPT_TIMER 32
#define ARCH_INTERRUPT_KEYBOARD 33
#define ARCH_PAGE_SIZE PAGE_SIZE

#define ARCH_SYSCALL(syscall) {\
    uint32_t s = syscall;\
    asm("mov %0, %%eax":: "r"(s));\
    asm("int $128");\
}

#define ARCH_SAVE_RET_ADDR(state) {\
    asm("mov 4(%%ebp), %0": "=r"((state)->eip));\
}

#define ARCH_SAVE_STACK_POINTER(state) {\
    asm("mov %ebp, %edx");\
    asm("add -4, %%edx");\
    asm("mov %%edx, %0": "=r"((state)->esp));\
    asm("mov (%%esp), %0": "=r"((state)->ebp));\
}

#endif //JAQ_ARCH_TYPES_H
