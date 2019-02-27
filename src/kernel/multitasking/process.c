//
// Created by sam on 10/02/19.
//

#include <multitasking/process.h>
#include <mem/mem.h>
#include <lib/string.h>

process_t *process_create(char *name, arch_cpu_state_t *kernel_state, arch_cpu_state_t* user_state, void *kernel_stack,
                          uint32_t kernel_stack_size, void* user_stack, uint32_t user_stack_size, process_level_t level) {
    process_t* process = kmalloc(sizeof(process_t));
    process->name = strdup(name);
    process->kernel_state = kernel_state;
    process->user_state = user_state;
    process->kernel_stack = kernel_stack;
    process->kernel_stack_size = kernel_stack_size;
    process->user_stack = user_stack;
    process->user_stack_size = user_stack_size;
    process->level = level;
    process->process_state = READY;
    return process;
}

void process_free(process_t *process) {
    kfree(process->kernel_state);
    kfree(process->kernel_stack);
    if (process->level == USER) {
        kfree(process->user_state);
        kfree(process->user_stack);
    }
    kfree(process->name);
}

