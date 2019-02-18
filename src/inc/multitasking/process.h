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

typedef struct process {
    char* name;
    arch_cpu_state_t* cpu_state;
    page_directory_t* page_directory;
    void* kernel_stack;
    uint32_t kernel_stack_size;
	void* user_stack;
	uint32_t user_stack_size;
    process_level_t level;
    process_state_t state;
} process_t;

process_t *process_create(char *name, arch_cpu_state_t *cpu_state, void *kernel_stack,
                          uint32_t kernel_stack_size, void* user_stack, uint32_t user_stack_size, process_level_t level);

void process_free(process_t* process);

#endif //JAQ_PROCESS_H
