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

#define ARCH_INTERRUPT_TIMER 32
#define ARCH_INTERRUPT_KEYBOARD 33
#define ARCH_PAGE_SIZE PAGE_SIZE

#define ARCH_SYSCALL(syscall) {\
    uint32_t s = syscall;\
    asm("mov %0, %%eax":: "r"(s));\
    asm("int $128");\
}

#define ARCH_INIT_PROCESS_STATE(process, entry_function, exit_function) {\
    arch_cpu_state_t* state = process->cpu_state;\
    uint32_t* process_kernel_stack = (uint32_t*)process->kernel_stack;\
    state->ebp = (uint32_t)process_kernel_stack;\
    state->esp = (uint32_t)process_kernel_stack + process->kernel_stack_size - 4*5;\
    state->edi = 0;\
    state->ebx = 0;\
    state->esi = 0;\
    process_kernel_stack[process->kernel_stack_size/4 - 5] = state->ebp;\
    process_kernel_stack[process->kernel_stack_size/4 - 4] = state->edi;\
    process_kernel_stack[process->kernel_stack_size/4 - 3] = state->esi;\
    process_kernel_stack[process->kernel_stack_size/4 - 2] = state->ebx;\
    process_kernel_stack[process->kernel_stack_size/4 - 1] = (uint32_t)entry_function;\
    process_kernel_stack[process->kernel_stack_size/4] = (uint32_t)exit_function;\
}

#endif //JAQ_ARCH_TYPES_H
