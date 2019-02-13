//
// Created by sam on 10/02/19.
//

#ifndef JAQ_MULTITASKING_H
#define JAQ_MULTITASKING_H

#include <arch_defs.h>
#include <multitasking/process.h>
#include <driver/timer.h>

#define MULTITASKING_TICK_TIME (1000 / TIMER_FREQUENCY)
#define MULTITASKING_TICKS_PER_SLICE 1

typedef struct sleeping_process {
    process_t* process;
    uint32_t wake_time;
} sleeping_process_t;

/**
 * Initialise the multitasking system. This will create its internal process queue and register
 * the timer handler
 */
void multitasking_init(void* kernel_stack, uint32_t kernel_stack_size);

/**
 * Add a process to the process queue, ready to be switched to
 * @param process The process to schedule
 */
void multitasking_schedule(process_t* process);

/**
 * Initialise a process' state so that it is ready to be switched to
 * @param process The process to initialise
 * @param entry_function The function to run when the task is switched to for the first time
 */
void multitasking_init_process_state(process_t *process, void (*entry_function)(void));

/**
 * Initialise a process' state so that it is ready to be switched to
 * @param process The process to initialise
 * @param entry_function The function to run when the task is switched to for the first time
 */
void multitasking_init_process_state(process_t *process, void (*entry_function)(void));

/**
 * Switch from the current process if there is one ready. If there are no ready processes then
 * no switching will occur
 */
void multitasking_yield();

/**
 * Exit the current process. Sets it as terminated ready to be cleaned and switches to the next ready process
 */
void multitasking_exit_process();

/**
 * Sleep the current process for at least the given number of milliseconds
 * @param milliseconds The smallest number of milliseconds the process will sleep for
 */
void multitasking_sleep(uint32_t milliseconds);

/**
 * Block a process so that it can't be switched to until unblocked
 * @param process The process to block. If it is the current process, then switching may occur
 */
void multitasking_block(process_t* process);

/**
 * Unblock a process so that it can be switched to
 * @param process The process to unblock. Can be the current process but behaviour is undefined
 */
void multitasking_unblock(process_t* process);

void multitasking_exit_process();

#endif //JAQ_MULTITASKING_H
