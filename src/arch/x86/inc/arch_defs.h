//
// Created by sam on 26/01/19.
//

#ifndef JAQ_ARCH_TYPES_H
#define JAQ_ARCH_TYPES_H

#include <stdinc.h>

#define ARCH "x86"

/**
 * The x86 registers saved on interrupt. Changes to this may require changes to multitasking.s and idt_asm.s
 */
struct arch_cpu_state {
    unsigned int cr3;
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
#define ARCH_PAGE_SIZE 0x400000
#define ARCH_IS_PAGE_ALIGNED(addr) ((addr % ARCH_PAGE_SIZE) == 0)
#define ARCH_ALIGN_DOWN(addr, alignment) ((addr) - ((addr) % alignment))
#define ARCH_ALIGN_UP(addr, alignment) (ARCH_ALIGN_DOWN(addr, alignment) + alignment)

#define ARCH_SYSCALL(syscall) {\
    uint32_t s = syscall;\
    asm("mov %0, %%eax":: "r"(s));\
    asm("int $128");\
}

#endif //JAQ_ARCH_TYPES_H
