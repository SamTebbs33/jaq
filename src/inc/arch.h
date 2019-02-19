//
// Created by sam on 26/01/19.
//

#ifndef JAQ_ARCH_H
#define JAQ_ARCH_H

#include <stdinc.h>
#include <arch_defs.h>
#include <multiboot.h>
#include <multitasking/process.h>

/**
 * The arch-specific initialisation function. Should be implemented by the architecture.
 * @param total_mem The total amount of memory available to the machine
 * @param mb_info The pointer to the multiboot info passed by grub
 * @param virtual_start The virtual start of the kernel code in memory
 * @param virtual_end The virtual end of the kernel code in memory
 * @param phys_start The physical start of the kernel code in memory
 * @param phys_end The physical end of the kernel code in memory
 * @param initrd_phys_end The physical address of the end of the initrd
 */
void arch_init(uint32_t total_mem, multiboot_info_t* mb_info, void* kernel_stack, uint32_t kernel_stack_size, uint32_t virtual_start, uint32_t virtual_end, uint32_t phys_start, uint32_t phys_end, uint32_t initrd_phys_end);

/**
 * Arch-specific implementation of outputting a byte over a port
 * @param port The port to output to
 * @param val The byte to output
 */
void arch_outb(uint16_t port, uint8_t val);

/**
 * Arch-specific implementation of receiving a byte from a port
 * @param port The port to receive over
 * @return The byte received
 */
uint8_t arch_inb(uint16_t port);

/**
 * Arch-specific implementation of registering an interrupt handler.
 * Get the interrupt number by including arch_types.h. An arch should define this and
 * the interrupts that it exposes
 * @param interrupt
 * @param handler
 */
void arch_register_interrupt_handler(int interrupt, arch_interrupt_handler_t handler);

/**
 * Sets the current page directory
 * @param page_dir The page directory to change to
 */
void arch_set_page_directory(page_directory_t* page_dir);

void arch_switch_task(arch_cpu_state_t* current, arch_cpu_state_t* next);

void arch_switch_user_task(arch_cpu_state_t* current, arch_cpu_state_t* next);

void arch_acknowledge_irq(int32_t irq);

void arch_init_process_state(process_t* process, void (*entry_function)(void), void (*exit_function)(void));

void arch_register_syscall(uint32_t syscall, arch_syscall_handler_t handler);

#endif //JAQ_ARCH_H
