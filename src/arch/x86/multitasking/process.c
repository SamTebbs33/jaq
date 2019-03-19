//
// Created by sam on 16/02/19.
//

#include <multitasking/process.h>
#include <gdt.h>
#include <lib/util.h>

extern void irq_return(void);

void arch_init_process_state(process_t* process, void (*entry_function)(void), void (*exit_function)(void)) {
    arch_cpu_state_t* kernel_state = process->kernel_state;
    arch_cpu_state_t* user_state = process->user_state;
    uint32_t* kernel_stack = (uint32_t*)process->kernel_stack;
    uint32_t kernel_stack_size = process->kernel_stack_size;
    memset(kernel_state, 0, sizeof(arch_cpu_state_t));

    if (process->level == USER) {
        uint32_t* user_stack = (uint32_t*)process->user_stack;
        uint32_t user_stack_size = process->user_stack_size;
        memset(user_state, 0, sizeof(arch_cpu_state_t));

        user_state->ss = user_state->gs = user_state->fs = user_state->es = user_state->ds = GDT_DATA_SEGMENT_USER;
        user_state->cs = GDT_CODE_SEGMENT_USER;
        user_state->useresp = (uint32_t)process->user_stack + process->user_stack_size;
        user_state->eflags = 0x200;
        user_state->eip = (uint32_t)entry_function;

        user_state->esp = user_state->useresp;
        user_state->ebp = 0;
        user_stack[process->user_stack_size/4] = (uint32_t)exit_function;
        // Exit return address
        kernel_stack[kernel_stack_size / 4] = (uint32_t) exit_function;
        // Put user state on stack
        memcpy((uint32_t*)((uint32_t)kernel_stack + kernel_stack_size - 4 - sizeof(arch_cpu_state_t)), user_state, sizeof(arch_cpu_state_t));
        logf(LOG_LEVEL_DEBUG, "copying to %d\n", (uint32_t)kernel_stack + kernel_stack_size - 4 - sizeof(arch_cpu_state_t));
        // Dummy eax value
        kernel_stack[kernel_stack_size / 4 - sizeof(arch_cpu_state_t) / 4 - 2] = 0;
        // Put address of routine that pops off the user state and enters user mode at bottom of the stack
        kernel_stack[kernel_stack_size / 4 - sizeof(arch_cpu_state_t) / 4 - 3] = (uint32_t) irq_return;
        // Point kernel esp to below exit function address, user state and dummy eax value
        kernel_state->esp = (uint32_t)kernel_stack + kernel_stack_size - sizeof(arch_cpu_state_t) - 3 * 4;
    } else {
        kernel_state->esp = (uint32_t)kernel_stack + kernel_stack_size - 4;
        kernel_stack[kernel_stack_size/4 - 1] = (uint32_t)entry_function;
        kernel_stack[kernel_stack_size/4] = (uint32_t)exit_function;
    }
    kernel_state->ebp = 0;
}
