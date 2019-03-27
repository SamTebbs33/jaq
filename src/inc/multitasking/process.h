//
// Created by sam on 10/02/19.
//

#ifndef JAQ_PROCESS_H
#define JAQ_PROCESS_H

#include <arch_defs.h>

typedef enum process_level {
    USER, KERNEL
} process_level_t;

typedef enum process_state {
    RUNNING, READY, TERMINATED, SLEEPING, BLOCKED
} process_state_t;

typedef struct process_stack {
    void* ptr;
    size_t size;
} process_stack_t;

typedef struct process {
    char* name;
    arch_cpu_state_t* kernel_state;
    arch_cpu_state_t* user_state;
    page_directory_t* page_directory;
    process_stack_t* kernel_stack;
    process_stack_t* user_stack;
    process_level_t level;
    process_state_t process_state;
} process_t;

process_t *process_create(char *name, arch_cpu_state_t *kernel_state, arch_cpu_state_t* user_state, process_stack_t* kernel_stack,
                          process_stack_t* user_stack, process_level_t level);

process_stack_t* process_stack_create(size_t size);

void process_free(process_t* process);

#endif //JAQ_PROCESS_H
