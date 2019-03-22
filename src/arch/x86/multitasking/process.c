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

        user_stack[255] = (uint32_t)exit_function;

        user_state->useresp = (uint32_t)process->user_stack + 255 * 4;
        user_state->esp = user_state->useresp;
        user_state->ebp = 0;

        kernel_stack[255] = (uint32_t)exit_function;
        kernel_stack[254] = user_state->ss;
        kernel_stack[253] = user_state->useresp;
        kernel_stack[252] = user_state->eflags;
        kernel_stack[251] = user_state->cs;
        kernel_stack[250] = user_state->eip;
        kernel_stack[249] = user_state->err_code;
        kernel_stack[248] = user_state->int_no;
        kernel_stack[247] = user_state->eax;
        kernel_stack[246] = user_state->ecx;
        kernel_stack[245] = user_state->edx;
        kernel_stack[244] = user_state->ebx;
        kernel_stack[243] = user_state->esp;
        kernel_stack[242] = user_state->ebp;
        kernel_stack[241] = user_state->esi;
        kernel_stack[240] = user_state->edi;
        kernel_stack[239] = user_state->ds;
        kernel_stack[238] = user_state->es;
        kernel_stack[237] = user_state->fs;
        kernel_stack[236] = user_state->gs;
        kernel_stack[235] = 0;
        kernel_stack[234] = (uint32_t)irq_return;

        kernel_state->esp = (uint32_t)kernel_stack + 234 * 4;

    } else {
        kernel_stack[255] = (uint32_t)exit_function;
        kernel_stack[254] = (uint32_t)entry_function;

        kernel_state->esp = (uint32_t)kernel_stack + 254 * 4;
    }
    kernel_state->ebp = 0;
}
