//
// Created by sam on 16/02/19.
//

#include <multitasking/process.h>
#include <gdt.h>
#include <lib/util.h>

extern void switch_to_user_task(arch_cpu_state_t* current, arch_cpu_state_t* next, uint32_t k_stack_base);
extern void switch_to_kernel_task(arch_cpu_state_t* current, arch_cpu_state_t* next);
extern void irq_return(void);

void arch_switch_task(process_t* current, process_t* next) {
    uint32_t kernel_stack_base = (uint32_t)next->kernel_stack + next->kernel_stack_size - 4;
    if(next->level == USER) {
        logf(LOG_LEVEL_DEBUG, "Switching to user task %s\n", next->name);
        switch_to_user_task(current->kernel_state, next->kernel_state, kernel_stack_base);
    }
    if(next->level == KERNEL) {
        logf(LOG_LEVEL_DEBUG, "Switching to kernel task %s\n", next->name);
        switch_to_kernel_task(current->kernel_state, next->kernel_state);
    }
}


/*
 * Process stackmap
 *
 * Kernel process:
 *
 *      Kernel stack
 *      top - 4: Exit function
 *      top - 8: Entry function  <- esp
 *
 *      No user stack
 *
 * User process:
 *
 *      Kernel stack
 *      top - 4: Exit function
 *      top - 8: Initial user state
 *      top - 8 - sizeof(arch_cpu_state_t): Unused eax value
 *      top - 12 - sizeof(arch_cpu_state_t): Return address  <- kernel_state.esp
 *
 *      User stack
 *      top - 4: Exit function  <- user_state.useresp
 *
 * The return address on the bottom of the kernel stack is set up to point to irq_return, which pops off the user state and does an iret into userland. Doing a ret from arch_switch_to_user_task will jump to irq_return.
 *
 */

void arch_init_process_state(process_t* process, void (*entry_function)(void), void (*exit_function)(void)) {
    arch_cpu_state_t* kernel_state = process->kernel_state;
    uint32_t* kernel_stack = (uint32_t*)process->kernel_stack;
    uint32_t kernel_stack_size = process->kernel_stack_size, kernel_stack_size_int = kernel_stack_size / 4;
    memset(kernel_state, 0, sizeof(arch_cpu_state_t));

    if (process->level == USER) {
        arch_cpu_state_t* user_state = process->user_state;
        uint32_t* user_stack = (uint32_t*)process->user_stack;
        uint32_t user_stack_size = process->user_stack_size, user_stack_size_int = user_stack_size / 4;
        memset(user_state, 0, sizeof(arch_cpu_state_t));

        // This state is popped off by the irq return code, which then does an iret into userland
        user_state->ss = user_state->gs = user_state->fs = user_state->es = user_state->ds = GDT_DATA_SEGMENT_USER;
        user_state->cs = GDT_CODE_SEGMENT_USER;
        user_state->eflags = 0x200;
        user_state->eip = (uint32_t)entry_function;

        user_stack[user_stack_size_int - 1] = (uint32_t)exit_function;

        user_state->useresp = (uint32_t)process->user_stack + (kernel_stack_size_int - 1) * 4;
        user_state->esp = user_state->useresp;
        user_state->ebp = 0;

        kernel_stack[kernel_stack_size_int - 2] = (uint32_t)exit_function;
        memcpy((void*)((uint32_t)kernel_stack + kernel_stack_size - sizeof(arch_cpu_state_t) - 8), user_state, sizeof(arch_cpu_state_t));

        kernel_stack[kernel_stack_size_int - 2 - sizeof(arch_cpu_state_t) / 4 - 1] = 0;
        kernel_stack[kernel_stack_size_int - 2 - sizeof(arch_cpu_state_t) / 4 - 2] = (uint32_t)irq_return;

        kernel_state->esp = (uint32_t)kernel_stack + kernel_stack_size - 8 - sizeof(arch_cpu_state_t) - 8;

    } else {
        kernel_stack[kernel_stack_size_int - 1] = (uint32_t)exit_function;
        kernel_stack[kernel_stack_size_int - 2] = (uint32_t)entry_function;

        kernel_state->esp = (uint32_t)kernel_stack + (kernel_stack_size_int - 2) * 4;
    }
    kernel_state->ebp = 0;
}
