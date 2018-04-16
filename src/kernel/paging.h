//
// Created by Samuel Tebbs on 13/04/2018.
//

#ifndef JAQ_PAGING_H
#define JAQ_PAGING_H

#include "stdint.h"
#include "stdbool.h"

#define BYTES_PER_PAGE 0x1000
#define PAGES_PER_TABLE 1024
#define TABLES_PER_DIRECTORY 1024

typedef struct {
    uint32_t present : 1;
    uint32_t writable : 1;
    uint32_t user_level : 1;
    uint32_t accessed : 1;
    uint32_t dirty : 1;
    uint32_t unused : 7;
    uint32_t frame : 20;
} page_t;

typedef struct {
    page_t pages[PAGES_PER_TABLE];
} page_table_t;

typedef struct {
    page_table_t* tables[TABLES_PER_DIRECTORY];
    uint32_t tables_physical[TABLES_PER_DIRECTORY];
    uint32_t physical_addr;
} page_directory_t;

page_directory_t* kernel_directory;

void paging_init(uint32_t mem_size);
void paging_set_directory(page_directory_t* new);
page_t* paging_get_page(uint32_t addr, page_directory_t* page_dir, bool create);
void alloc_frame(page_t* page, bool is_kernel, bool is_writeable);
void free_frame(page_t* page);

#endif //JAQ_PAGING_H
