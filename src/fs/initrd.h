//
// Created by Samuel Tebbs on 17/04/2018.
//

#ifndef JAQ_INITRD_H
#define JAQ_INITRD_H

#include <stdinc.h>
#include "fs.h"
#include <multiboot.h>

#define INITRD_FILENAME_MAX 64

typedef struct {
    char name[INITRD_FILENAME_MAX];
    uint32_t length;
    uint32_t offset;
} initrd_file_t;

typedef struct {
    uint32_t n_nodes;
} initrd_t;

fs_node_t* initrd_init(uint32_t initrd_addr);

#endif //JAQ_INITRD_H
