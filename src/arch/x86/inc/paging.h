//
// Created by sam on 24/04/18.
//

#ifndef JAQ_PAGING2_H
#define JAQ_PAGING2_H

#include <stdinc.h>

#define PAGE_SIZE 4096
#define PAGE_TABLE_ENTRIES_PER_DIRECTORY 1024
#define PAGE_ENTRIES_PER_TABLE 1024
#define PAGES_PER_DIRECTORY (PAGE_TABLE_ENTRIES_PER_DIRECTORY * PAGE_ENTRIES_PER_TABLE)

#define ALIGN_DOWN(addr) ((addr) - ((addr) % PAGE_SIZE))
#define ALIGN_UP(addr) (ALIGN_DOWN(addr) + PAGE_SIZE)
#define IS_PAGE_ALIGNED(addr) ((addr | 0xFFFFF000) == 0xFFFFF000)

typedef struct {
    uint8_t present : 1;
    uint8_t writable : 1;
    uint8_t user_level : 1;
    uint8_t write_through : 1;
    uint8_t caching_disabled : 1;
    uint8_t accessed : 1;
    uint8_t dirty : 1;
    uint8_t zero : 1;
    uint8_t global : 1;
    uint8_t ignored : 3;
    uint32_t physical_addr : 20; // 4KiB aligned physical address of page
} page_table_entry_t;

typedef struct {
    page_table_entry_t entries[PAGE_ENTRIES_PER_TABLE];
} page_table_t;

typedef struct {
    uint8_t present : 1;
    uint8_t writable : 1;
    uint8_t user_level : 1;
    uint8_t write_through : 1;
    uint8_t caching_disabled : 1;
    uint8_t accessed : 1;
    uint8_t zero : 1;
    uint8_t four_megabyte_pages : 1;
    uint8_t global : 1;
    uint8_t ignored : 3;
    uint32_t table_physical_addr : 20; // 4KiB aligned physical addr of page table
} page_dir_entry_t;

typedef struct {
    page_dir_entry_t entries[PAGE_TABLE_ENTRIES_PER_DIRECTORY];
    page_table_t* tables[PAGE_TABLE_ENTRIES_PER_DIRECTORY];
} page_directory_t;

typedef void (*page_dir_entry_callback_t)(page_dir_entry_t* entry, page_table_t* table);
typedef void (*page_table_entry_callback_t)(page_table_entry_t* entry);

/**
 * Initialise the paging structures
 * @param total_mem The total memory to set up paging for
 * @param initrd_end The end of the initial ramdisk. This is where memory allocation takes place before a heap is set up.
 */
void paging_init(uint32_t total_mem, uint32_t initrd_end);

/**
 * Get the page associated with a given memory address
 * @param addr The virtual address
 * @param directory The directory in which to search
 * @return The page
 */
page_table_entry_t *paging_get_page(uint32_t addr, page_directory_t *directory);

/**
 * Get the page table associated with a given memory address
 * @param addr The virtual address
 * @return The table
 */
page_dir_entry_t* paging_get_table(uint32_t addr);

/**
 * Set the current paging directory to be used by the MMU. Causes a TLB flush
 * @param directory The directory
 */
void paging_set_directory(page_directory_t *directory);

/**
 * Deallocate a page so that it can be allocated some other time. Mark it as non-present
 * @param page The page to free
 */
void paging_free_page(page_table_entry_t *page);

/**
 * Allocate a page to some physical memory space and mark it as present
 * @param page The page to allocate
 * @return True if allocation succeeded, else false
 */
bool paging_alloc_page(page_table_entry_t *page);

/**
 * Create a new page directory
 * @param phys_start The start of the physical address space
 * @param phys_end The end of the physical address space
 * @param virtual_start The start of the virtual address space to which to map
 * @param virtual_end The end of the virtual address space to which to map
 * @return The directory created
 */
page_directory_t* paging_create_directory(uint32_t phys_start, uint32_t phys_end, uint32_t virtual_start, uint32_t virtual_end);

#endif //JAQ_PAGING2_H
