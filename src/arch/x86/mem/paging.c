//
// Created by sam on 24/04/18.
//

#include <idt.h>
#include <paging.h>
#include <mem.h>
#include <string.h>
#include <log.h>
#include <heap.h>
#include <maths.h>
#include <util.h>
#include <boot_constants.h>

#define HEAP_INDEX_SIZE 0x20000

page_directory_t* kernel_directory = NULL;
heap_t* kernel_heap = NULL;
extern uint32_t pile_start, pile_end, pile_ptr;

void page_fault_handler(arch_registers_t* registers) {
    uint32_t err_code = registers->err_code;
    bool protection_err = (bool)(err_code & PAGE_FAULT_PROTECTION), write_err = (bool)(err_code & PAGE_FAULT_WRITE),
        user_err = (bool)(err_code & PAGE_FAULT_USER), reserved_err = (bool)(err_code & PAGE_FAULT_RESERVED),
        exec_err = (bool)(err_code & PAGE_FAULT_EXEC);
    uint32_t bad_addr;
    asm ("mov %%cr2, %0": "=r"(bad_addr));
    PANIC("Page fault for virtual address %d: %s%s%s, %s, %s\n", bad_addr, reserved_err ? "reserved, " : "", exec_err ? "execution, " : "",
            protection_err ? "protection" : "non-present", write_err ? "write" : "read", user_err ? "user" : "supervisor");
}

void paging_init(uint32_t mem_kilobytes, uint32_t virtual_start, uint32_t virtual_end, uint32_t phys_start, uint32_t phys_end, uint32_t initrd_end) {
    // Align the page size to PAGE_SIZE
    // so we don't make a page table that oversteps the total memory
    uint32_t mem_kilobytes_aligned = mem_kilobytes - (mem_kilobytes % PAGE_SIZE);

    /*
     * Memory map:
     *
     *                  Physical                                        Virtual
     *  0x000000        Grub                   -->  0x3221225472 (3GiB)         Grub
     *  0x100000 (1MiB) Kernel code start      -->  0x3222274048 (3GiB + 1MiB)  Kernel code start
     *  kernel_phys_end Kernel code end        -->  kernel_virt_end             Kernel code end
     *  0x??????        Initrd (modules) start -->  0x??????????                Initrd (modules) start
     *  initrd_end      Initrd (modules) end   -->  initrd_virt_end             Initrd (modules) end
     *  pile_phys_start Pile start             -->  pile_virt_start             Pile start
     *              KERNEL_PILE_SIZE                                KERNEL_PILE_SIZE
     *  pile_phys_end   Pile end               -->  pile_virt_end               Pile end
     *  heap_phys_start Heap start             -->  heap_virt_start             Heap start
     *              KERNEL_HEAP_SIZE                                KERNEL_HEAP_SIZE
     *  heap_phys_end   Heap end               -->  heap_virt_end               Heap end
     *
     *  We probably don't need to compute and store all of these values, but it helps with clarity (I think)
     */

    uint32_t code_phys_start = phys_start;
    uint32_t code_phys_end = phys_end;
    uint32_t code_virt_start = virtual_start;
    uint32_t code_virt_end = virtual_end;

    uint32_t initrd_phys_end = initrd_end;
    uint32_t initrd_phys_offset = initrd_phys_end - code_phys_end;

    uint32_t pile_phys_start = initrd_phys_end;
    uint32_t pile_phys_end = pile_phys_start + KERNEL_PILE_SIZE;
    uint32_t heap_phys_start = pile_phys_end;
    uint32_t heap_phys_end = heap_phys_start + KERNEL_HEAP_SIZE;
    uint32_t pile_virt_start = code_virt_end + initrd_phys_offset;
    uint32_t pile_virt_end = pile_virt_start + KERNEL_PILE_SIZE;
    uint32_t heap_virt_start = pile_virt_end;
    uint32_t heap_virt_end = heap_virt_start + KERNEL_HEAP_SIZE;

    uint32_t map_phys_start = ALIGN_DOWN(code_phys_start);
    uint32_t map_phys_end = ALIGN_UP(heap_phys_end);
    uint32_t map_virt_start = ALIGN_DOWN(code_virt_start);
    uint32_t map_virt_end = ALIGN_UP(heap_virt_end);

    pile_start = pile_virt_start;
    pile_ptr = pile_start;
    pile_end = pile_virt_end;

    idt_register_isr_handler(14, page_fault_handler);

    // Create and map the directory
    kernel_directory = kmalloc_a(sizeof(page_directory_t));
    paging_map_4mb_dir(kernel_directory, map_phys_start, map_phys_end, map_virt_start, map_virt_end);

    // Set directory
    uint32_t dir_physaddr = VIRTUAL_TO_PHYSICAL((uint32_t) kernel_directory);
    asm ("mov %0, %%cr3":: "r"(dir_physaddr));

    // Create the kernel heap
    kernel_heap = heap_create(heap_virt_start, heap_virt_end, true, true, HEAP_INDEX_SIZE);
}

void paging_map_4mb_page(page_directory_t* dir, uint32_t page, uint32_t phys_addr) {
    page_dir_entry_t* dir_entry = &dir->entries[page];
    dir_entry->accessed = 0;
    dir_entry->caching_disabled = 0;
    dir_entry->four_megabyte_pages = 1;
    dir_entry->available = 0;
    dir_entry->present = 1;
    dir_entry->user_level = 0;
    dir_entry->writable = 1;
    dir_entry->write_through = 1;
    ASSERT_PAGE_ALIGNED("table physical alignment", phys_addr);
    // Shift to isolate the top 10 bits (the 4MiB aligned physical address)
    dir_entry->page_physical_addr = phys_addr >> 22;
}

void paging_map_4mb_dir(page_directory_t* dir, uint32_t phys_start, uint32_t phys_end, uint32_t virtual_start, uint32_t virtual_end) {
    if (phys_start > phys_end) PANIC("Improper start and end physical addresses");
    if(!IS_PAGE_ALIGNED(phys_start) || !IS_PAGE_ALIGNED(phys_end)) PANIC("Physical addresses are not page aligned");
    if (virtual_start > virtual_end) PANIC("Improper start and end virtual addresses");
    if(!IS_PAGE_ALIGNED(virtual_start) || !IS_PAGE_ALIGNED(virtual_end)) PANIC("Virtual addresses are not page aligned");
    if(virtual_end - virtual_start != phys_end - phys_start) PANIC("Virtual space is not equal to physical space");

    if (dir) {
        memset(dir, 0, sizeof(page_directory_t));
        uint32_t virtual_addr = virtual_start, phys_addr = phys_start;
        uint32_t page = virtual_addr / PAGE_SIZE;

        while (page < PAGE_TABLE_ENTRIES_PER_DIRECTORY && virtual_addr <= virtual_end) {
            paging_map_4mb_page(dir, page, phys_addr);
            phys_addr += PAGE_SIZE;
            virtual_addr += PAGE_SIZE;
            page++;
        }
    }
}
