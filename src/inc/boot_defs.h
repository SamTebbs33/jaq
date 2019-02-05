//
// Created by samteb01 on 30/01/19.
//

#ifndef JAQ_BOOT_CONSTANTS_H
#define JAQ_BOOT_CONSTANTS_H

// Kernel virtual address offset from physical address
#define KERNEL_ADDR_OFFSET 0xC0000000
// 20 MiB kernel heap
#define KERNEL_HEAP_SIZE 0x1400000
// 4 MiB temp allocation space. Changing this will require changing number of kernel pages
#define KERNEL_PILE_SIZE 0x400000
#define VIRTUAL_TO_PHYSICAL(addr) ((addr) - KERNEL_ADDR_OFFSET)
#define PHYSICAL_TO_VIRTUAL(addr) ((addr) + KERNEL_ADDR_OFFSET)

#endif //JAQ_BOOT_CONSTANTS_H
