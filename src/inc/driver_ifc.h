//
// Created by Samuel Tebbs on 18/04/2018.
//

#ifndef JAQ_DRIVER_IFC_H
#define JAQ_DRIVER_IFC_H

#include <stdinc.h>
#include "../fs/fs.h"
#include "interrupts.h"

typedef struct {
    uint32_t (*kmalloc)(size_t size);
    uint32_t (*kmalloc_a)(size_t size);
    uint32_t (*kmalloc_p)(size_t size, uint32_t* phys);
    uint32_t (*kmalloc_ap)(size_t size, uint32_t* phys);
    void (*kfree)(void* ptr);
} mem_ifc_t;

typedef struct {
    fs_node_t* (*fs_make_node)(char* name, fs_node_perms_t perms, fs_node_flags_t flags, fs_node_user_t user, fs_node_group_t group,
                            fs_node_read_t read, fs_node_write_t write, fs_node_open_t open, fs_node_close_t close, fs_node_finddir_t finddir, fs_node_readdir_t readdir, size_t len, uint32_t impl);
    size_t (*fs_read)(fs_node_t* node, void* buff, size_t len, size_t offset);
    size_t (*fs_write)(fs_node_t* node, void* buff, size_t len, size_t offset);
    bool (*fs_open)(fs_node_t* node, char* mode);
    bool (*fs_close)(fs_node_t* node);
    fs_node_t* (*fs_readdir)(fs_node_t* node, uint32_t idx);
    fs_node_t* (*fs_finddir)(fs_node_t* node, char* child_name);
} fs_ifc_t;

typedef struct {
    void (*fb_putc)(unsigned char ch, unsigned char fg, unsigned char bg);
    void (*fb_cursor)(unsigned int row, unsigned int column);
    void (*fb_clear)(uint8_t fg, uint8_t bg);
} fb_ifc_t;

typedef struct {
    bool (*interrupts_register_handler)(uint8_t interrupt, interrupt_handler_t handler);
} int_ifc_t;

typedef struct {
    mem_ifc_t mem_ifc;
    fs_ifc_t fs_ifc;
    fb_ifc_t fb_ifc;
    int_ifc_t int_ifc;
} driver_ifc_t;

#endif //JAQ_DRIVER_IFC_H
