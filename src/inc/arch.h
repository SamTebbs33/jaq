//
// Created by sam on 26/01/19.
//

#ifndef JAQ_ARCH_H
#define JAQ_ARCH_H

#include <stdinc.h>
#include <arch_types.h>
#include <multiboot.h>

void arch_init(uint32_t total_mem, uint32_t initrd_end, multiboot_info_t* mb_info);
void arch_outb(uint16_t port, uint8_t val);
uint8_t arch_inb(uint16_t port);
void arch_register_interrupt_handler(int interrupt, arch_interrupt_handler_t handler);
#endif //JAQ_ARCH_H
