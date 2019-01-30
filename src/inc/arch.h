//
// Created by sam on 26/01/19.
//

#ifndef JAQ_ARCH_H
#define JAQ_ARCH_H

#include <stdinc.h>
#include <arch_types.h>
#include <multiboot.h>

/**
 * The arch-specific initialisation function. Should be implemented by the architecture.
 * @param total_mem The total amount of memory available to the machine
 * @param initrd_end The end of the initial ramdisk
 * @param mb_info The pointer to the multiboot info passed by grub
 */
void arch_init(uint32_t total_mem, uint32_t initrd_end, multiboot_info_t* mb_info);

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
#endif //JAQ_ARCH_H
