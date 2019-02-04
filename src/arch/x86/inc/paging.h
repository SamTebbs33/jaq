//
// Created by sam on 24/04/18.
//

#ifndef JAQ_PAGING2_H
#define JAQ_PAGING2_H

#include <stdinc.h>
#include <boot_constants.h>

#define PAGE_SIZE_4MB 0x400000
#define PAGE_SIZE_4KB 0x1000
#define PAGE_SIZE PAGE_SIZE_4MB
#define PAGE_TABLE_ENTRIES_PER_DIRECTORY 1024
#define PAGES_PER_DIRECTORY (PAGE_ENTRIES_PER_TABLE)

#define PAGE_FAULT_PROTECTION (1 << 0)
#define PAGE_FAULT_WRITE (1 << 1)
#define PAGE_FAULT_USER (1 << 2)
#define PAGE_FAULT_RESERVED (1 << 3)
#define PAGE_FAULT_EXEC (1 << 4)

#define ALIGN_DOWN(addr) ((addr) - ((addr) % PAGE_SIZE))
#define ALIGN_UP(addr) (ALIGN_DOWN(addr) + PAGE_SIZE)
#define IS_PAGE_ALIGNED(addr) ((addr % PAGE_SIZE) == 0)

typedef struct {
    uint8_t present : 1;
    uint8_t writable : 1;
    uint8_t user_level : 1;
    uint8_t write_through : 1;
    uint8_t caching_disabled : 1;
    uint8_t accessed : 1;
    uint8_t zero : 1;
    uint8_t four_megabyte_pages : 1;
    uint8_t ignored : 1;
    // Free for use by kernel
    uint8_t available : 3;
    uint16_t reserved : 10;
    // Free for use by kernel when present == 0. Could use to store address in swap space
    uint32_t page_physical_addr : 10;
} __attribute__((packed)) page_dir_entry_t;

typedef struct {
    page_dir_entry_t entries[PAGE_TABLE_ENTRIES_PER_DIRECTORY];
    // At the end of struct so should be ignored by MMU
    //page_table_t* tables[PAGE_TABLE_ENTRIES_PER_DIRECTORY];
} __attribute__((packed)) page_directory_t;

/**
 * Initialise the paging structures
 * @param mem_kilobytes The total memory, in kilobytes, to set up paging for
 * @param virtual_start The virtual start of the kernel code in memory
 * @param virtual_end The virtual end of the kernel code in memory
 * @param phys_start The physical start of the kernel code in memory
 * @param phys_end The physical end of the kernel code in memory
 * @param initrd_end The physical end of the initrd
 */
void paging_init(uint32_t mem_kilobytes, uint32_t virtual_start, uint32_t virtual_end, uint32_t phys_start, uint32_t phys_end, uint32_t initrd_end);

/**
 * Map a page directory with 4MiB entries from phys_start, phys_end to virtual_start and virtual_end
 * @param dir The directory to map
 * @param phys_start The initial physical address to map to
 * @param phys_end The physical address to map up to
 * @param virtual_start The initial virtual address to map from
 * @param virtual_end The virtual address to map up to
 */
void paging_map_4mb_dir(page_directory_t* dir, uint32_t phys_start, uint32_t phys_end, uint32_t virtual_start, uint32_t virtual_end);

/**
 * Map a page to a 4MiB aligned physical address
 * @param dir The directory containing the page to map
 * @param page The page within the directory to map
 * @param phys_addr The 4MiB aligned physical address
 */
void paging_map_4mb_page(page_directory_t* dir, uint32_t page, uint32_t phys_addr);

#endif //JAQ_PAGING2_H
