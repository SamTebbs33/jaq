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
    dest->eip = src->eip;
}

void arch_restore_cpu_state(arch_cpu_state_t* state) {
    asm("mov %0, %%eax":: "r"(state));
    asm("mov 16(%eax), %edi");
    asm("mov 20(%eax), %esi");
    asm("mov 32(%eax), %ebx");
    asm("mov 36(%eax), %edx");
    asm("mov 40(%eax), %ecx");
    asm("mov 44(%eax), %eax");
}

void arch_save_cpu_state(arch_cpu_state_t* state) {
    asm("push %eax");
    asm("mov %0, %%eax":: "r"(state));
    asm("mov %edi, 16(%eax)");
    asm("mov %esi, 20(%eax)");
    asm("mov %ebx, 32(%eax)");
    asm("mov %edx, 36(%eax)");
    asm("mov %ecx, 40(%eax)");
    asm("pop 44(%eax)");
}

void arch_init_process_state(process_t* process, void (*entry_function)(void), void (*exit_function)(void)) {
    arch_cpu_state_t* kernel_state = process->kernel_state;
    arch_cpu_state_t* user_state = process->user_state;
    uint32_t* process_kernel_stack = (uint32_t*)process->kernel_stack;
    memset(kernel_state, 0, sizeof(arch_cpu_state_t));

    if (process->level == USER) {
        memset(user_state, 0, sizeof(arch_cpu_state_t));
        kernel_state->useresp = (uint32_t)process->user_stack + process->user_stack_size;
        user_state->esp = kernel_state->useresp;
        user_state->ebp = user_state->esp - 4;
        uint32_t* user_stack = process->user_stack;
        user_state->eip = (uint32_t)entry_function;
        user_stack[process->user_stack_size/4] = (uint32_t)exit_function;
    }

    kernel_state->esp = (uint32_t)process_kernel_stack + process->kernel_stack_size - 4;
    kernel_state->ebp = kernel_state->esp - 4;
    process_kernel_stack[process->kernel_stack_size/4 - 1] = (uint32_t)entry_function;
    process_kernel_stack[process->kernel_stack_size/4] = (uint32_t)exit_function;
}
