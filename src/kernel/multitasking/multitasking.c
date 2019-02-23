//
// Created by sam on 10/02/19.
//

#include <multitasking/multitasking.h>
#include <lib/linkedlist.h>
#include <lib/queue.h>
#include <arch.h>
#include <multitasking/process.h>
#include <arch_defs.h>
#include <mem/mem.h>
#include <log/log.h>
#include <lib/sorted_linkedlist.h>

queue_t* process_queue = NULL, * unblocked_queue = NULL, * terminated_queue = NULL;
linkedlist_t* sleeping_processes = NULL;
process_t* current_process = NULL;
uint32_t tick_counter = 0;
uint32_t milliseconds_counter = 0;

process_t* init_process = NULL, * cleaner_process = NULL;
extern void (*arch_user_test)(void);

void on_tick(arch_cpu_state_t* state) {
    tick_counter++;
    arch_acknowledge_irq(ARCH_INTERRUPT_TIMER);
    if(tick_counter >= MULTITASKING_TICKS_PER_SLICE) {
        // Copy the interrupted state into the appropriate process state
        if (current_process->level == USER) arch_copy_cpu_state(current_process->user_state, state);
        else arch_copy_cpu_state(current_process->kernel_state, state);
        multitasking_yield();
        tick_counter = 0;
    }
    milliseconds_counter += MULTITASKING_TICK_TIME;
}

int compare_sleeping_processes(void* p1, void* p2) {
    return ((sleeping_process_t*)p1)->wake_time - ((sleeping_process_t*)p2)->wake_time;
}

void cleaner() {
    ARCH_SYSCALL(0x1);
    while (true) {
        if(queue_size(terminated_queue) > 0) {
            process_t* proc = queue_dequeue(terminated_queue);
            process_free(proc);
            kfree(proc);
            multitasking_sleep(100);
        }
    }
}

void multitasking_exit_process() {
    current_process->process_state = TERMINATED;
    queue_enqueue(terminated_queue, current_process);
    multitasking_yield();
}

void multitasking_init(void* kernel_stack, uint32_t kernel_stack_size) {
    process_queue = queue_create();
    sleeping_processes = linkedlist_create();
    unblocked_queue = queue_create();
    terminated_queue = queue_create();

    // Create the init process
    init_process = current_process = process_create("init", kmalloc(sizeof(arch_cpu_state_t)), NULL, kernel_stack, kernel_stack_size, NULL, 0, KERNEL);
    // The init process just carries on from when it was created so we don't set an entry point
    multitasking_init_process_state(current_process, 0);

    // Create the cleaner process
    cleaner_process = process_create("cleaner", kmalloc(sizeof(arch_cpu_state_t)), kmalloc(sizeof(arch_cpu_state_t)), kmalloc(1024), 1024, kmalloc(1024), 1024, USER);
    multitasking_init_process_state(cleaner_process, cleaner);
    multitasking_schedule(cleaner_process);

    arch_register_interrupt_handler(ARCH_INTERRUPT_TIMER, on_tick);
}

void multitasking_init_process_state(process_t *process, void (*entry_function)(void)) {
    arch_init_process_state(process, entry_function, multitasking_exit_process);
}

void multitasking_schedule(process_t *process) {
    if(process->process_state != TERMINATED) queue_enqueue(process_queue, process);
}

bool process_can_run(process_t* proc) {
    return proc->process_state != SLEEPING && proc->process_state != BLOCKED && proc->process_state != TERMINATED;
}

process_t *get_next_from_queue(queue_t *queue) {
    process_t* head = queue_dequeue(queue);
    // Cut off processes that aren't ready to run yet
    while (!process_can_run(head) && queue_size(queue) > 0) head = queue_dequeue(queue);
    return head;
}

void switch_to_next() {
    process_t* next_process = NULL;
    // Check the list of sleeping processes
    if(linkedlist_size(sleeping_processes) > 0) {
        //log_debug("More than 0 sleeping processes\n");
        sleeping_process_t* sp = linkedlist_get(sleeping_processes, 0);
        // If the first process in the list is ready to be woken up
        if(sp->wake_time <= milliseconds_counter) {
            //log_debug("Ready to wake first one\n");
            next_process = sp->process;
            next_process->process_state = READY;
            // Remove it from the list and free the wrapper struct
            linkedlist_remove(sleeping_processes, 0);
            kfree(sp);
        }
    }
    // Check the queue of unblocked processes
    if(!next_process && queue_size(unblocked_queue) > 0) next_process = get_next_from_queue(unblocked_queue);
    // Check the queue of ready processes
    if(!next_process && queue_size(process_queue) > 0) next_process = get_next_from_queue(process_queue);
    if(next_process && process_can_run(next_process)) {
        next_process->process_state = RUNNING;
        process_t *tmp = current_process;
        current_process = next_process;
        if(next_process->level == KERNEL) {
		log_debug("Switching to kernel task\n");
		arch_switch_to_kernel_task(tmp->kernel_state, current_process->kernel_state);
	}
        else {
		log_debug("Switching to user task\n");
		arch_switch_to_user_task(tmp->kernel_state, current_process->kernel_state, current_process->user_state);
	}
    }
}

void multitasking_yield() {
    multitasking_schedule(current_process);
    current_process->process_state = READY;
    switch_to_next();
}

void multitasking_sleep(uint32_t milliseconds) {
    sleeping_process_t* sp = kmalloc(sizeof(sleeping_process_t));
    if(sp) {
        sp->process = current_process;
        sp->wake_time = milliseconds_counter + milliseconds;
        sorted_linkedlist_add(sleeping_processes, sp, compare_sleeping_processes);
        // Setting it to sleeping state will get it cut from the process queue if it's ever at the front
        current_process->process_state = SLEEPING;
        switch_to_next();
    }
}

void multitasking_block(process_t *process) {
    process->process_state = BLOCKED;
    if(process->process_state == RUNNING || current_process == process) {
        switch_to_next();
    }
}

void multitasking_unblock(process_t *process) {
    if(process->process_state != TERMINATED) {
        process->process_state = READY;
        multitasking_schedule(process);
    }
}
