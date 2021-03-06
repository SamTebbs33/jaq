//
// Created by sam on 10/02/19.
//

#include <multitasking/process.h>
#include <mem/mem.h>
#include <lib/string.h>

process_t *process_create(char *name, arch_cpu_state_t *cpu_state, void *kernel_stack,
                          uint32_t kernel_stack_size, process_level_t level) {
    process_t* process = kmalloc(sizeof(process_t));
    process->name = strdup(name);
    process->cpu_state = cpu_state;
    process->kernel_stack = kernel_stack;
    process->kernel_stack_size = kernel_stack_size;
    process->level = level;
    process->state = READY;
    return process;
}

void process_free(process_t *process) {
    kfree(process->cpu_state);
    kfree(process->kernel_stack);
    kfree(process->name);
}

