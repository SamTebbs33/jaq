//
// Created by sam on 16/02/19.
//

#include <multitasking/process.h>
#include <lib/util.h>

void arch_copy_cpu_state(arch_cpu_state_t* dest, arch_cpu_state_t* src) {
    dest->edi = src->edi;
    dest->esi = src->esi;
    dest->ebx = src->ebx;
    dest->ebp = src->ebp;
    dest->edx = src->edx;
    dest->ecx = src->ecx;
    dest->eax = src->eax;
}

void arch_init_process_state(process_t* process, void (*entry_function)(void), void (*exit_function)(void)) {
    arch_cpu_state_t* kernel_state = process->kernel_state;
    arch_cpu_state_t* user_state = process->user_state;
    uint32_t* process_kernel_stack = (uint32_t*)process->kernel_stack;
    memset(kernel_state, 0, sizeof(arch_cpu_state_t));

    if (process->level == USER) {
        memset(user_state, 0, sizeof(arch_cpu_state_t));
        kernel_state->useresp = (uint32_t)process->user_stack + process->user_stack_size - 4;
        user_state->esp = kernel_state->useresp;
        user_state->ebp = user_state->esp - 4;
        uint32_t* user_stack = process->user_stack;
        user_stack[process->user_stack_size/4 - 1] = (uint32_t)entry_function;
        user_stack[process->user_stack_size/4] = (uint32_t)exit_function;
    }

    kernel_state->esp = (uint32_t)process_kernel_stack + process->kernel_stack_size - 4;
    kernel_state->ebp = kernel_state->esp - 4;
    process_kernel_stack[process->kernel_stack_size/4 - 1] = (uint32_t)entry_function;
    process_kernel_stack[process->kernel_stack_size/4] = (uint32_t)exit_function;
}
