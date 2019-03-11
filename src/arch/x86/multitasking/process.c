#include <multitasking/process.h>
#include <lib/string.h>

void arch_init_process_state(process_t* process, void (*entry_function)(void), void (*exit_function)(void)) {
    arch_cpu_state_t* kernel_state = process->cpu_state;
    uint32_t* process_kernel_stack = (uint32_t*)process->kernel_stack;
    memset(kernel_state, 0, sizeof(arch_cpu_state_t));

    kernel_state->esp = (uint32_t)process_kernel_stack + process->kernel_stack_size - 4;
    kernel_state->ebp = kernel_state->esp - 4;
    kernel_state->eip = (uint32_t)entry_function;
    process_kernel_stack[process->kernel_stack_size/4] = (uint32_t)exit_function;
}
