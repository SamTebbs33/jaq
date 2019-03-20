//
// Created by sam on 16/02/19.
//

#include <multitasking/process.h>
#include <gdt.h>
#include <lib/util.h>

extern void irq_return(void);

/*
 * Process stackmap
 *
 * Kernel process:
 *
 *      Kernel stack
 *      top: Exit function
 *      top - 4: Entry function  <- esp
 *
 *      No user stack
 *
 * User process:
 *
 *      Kernel stack
 *      top: Exit function
 *      top - 4: Initial user state
 *      top - 4 - sizeof(arch_cpu_state_t): Unused eax value
 *      top - 8 - sizeof(arch_cpu_state_t): Return address  <- kernel_state.esp
 *
 *      User stack
 *      top: Exit function  <- user_state.useresp
 *
 * The return address on the bottom of the kernel stack is set up to point to irq_return, which pops off the user state and does an iret into userland. Doing a ret from arch_switch_to_user_task will jump to irq_return.
 *
 */

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

        // This state is popped off by the irq return code, which then does an iret into userland
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
        // Dummy eax value
        kernel_stack[kernel_stack_size / 4 - sizeof(arch_cpu_state_t) / 4 - 2] = 0;
        // Put address of routine that pops off the user state and enters user mode
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
