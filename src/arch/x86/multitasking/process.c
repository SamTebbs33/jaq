//
// Created by sam on 16/02/19.
//

#include <multitasking/process.h>
#include <lib/util.h>

void arch_init_process_state(process_t* process, void (*entry_function)(void), void (*exit_function)(void)) {
    arch_cpu_state_t* state = process->cpu_state;
    uint32_t* process_kernel_stack = (uint32_t*)process->kernel_stack;
    memset(state, 0, sizeof(arch_cpu_state_t));

    if (process->level == USER) {
        state->useresp = (uint32_t)process->user_stack + process->user_stack_size;
    }

    state->ebp = (uint32_t)process_kernel_stack;
    state->esp = (uint32_t)process_kernel_stack + process->kernel_stack_size - 4*5;
    process_kernel_stack[process->kernel_stack_size/4 - 5] = state->edi;
    process_kernel_stack[process->kernel_stack_size/4 - 4] = state->ebp;
    process_kernel_stack[process->kernel_stack_size/4 - 3] = state->esi;
    process_kernel_stack[process->kernel_stack_size/4 - 2] = state->ebx;
    process_kernel_stack[process->kernel_stack_size/4 - 1] = (uint32_t)entry_function;
    process_kernel_stack[process->kernel_stack_size/4] = (uint32_t)exit_function;
}
