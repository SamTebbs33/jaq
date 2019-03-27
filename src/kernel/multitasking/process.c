//
// Created by sam on 10/02/19.
//

#include <multitasking/process.h>
#include <mem/mem.h>
#include <lib/string.h>

process_t *process_create(char *name, arch_cpu_state_t *kernel_state, arch_cpu_state_t* user_state, process_stack_t* kernel_stack,
                          process_stack_t* user_stack, process_level_t level) {
    process_t* process = kmalloc(sizeof(process_t));
    process->name = strdup(name);
    process->kernel_state = kernel_state;
    process->user_state = user_state;
    process->kernel_stack = kernel_stack;
    process->user_stack = user_stack;
    process->level = level;
    process->process_state = READY;
    return process;
}

process_stack_t* process_stack_create(size_t size) {
    process_stack_t* stack = kmalloc(sizeof(process_stack_t));
    if(stack) {
        stack->ptr = kmalloc(size);
        if(!stack->ptr) {
            kfree(stack);
            return NULL;
        }
        stack->size = size;
    }
    return stack;
}

void process_stack_free(process_stack_t* stack) {
    kfree(stack->ptr);
}

void process_free(process_t *process) {
    kfree(process->kernel_state);
    process_stack_free(process->kernel_stack);
    kfree(process->kernel_stack);
    if (process->level == USER) {
        kfree(process->user_state);
        process_stack_free(process->user_stack);
        kfree(process->user_stack);
    }
    kfree(process->name);
}

