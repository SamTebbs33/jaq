//
// Created by Samuel Tebbs on 17/04/2018.
//

#include "fs.h"
#include "../kernel/mem.h"
#include "../kernel/string.h"

fs_node_t * fs_make_node(char *name, fs_node_perms_t perms, fs_node_flags_t flags, fs_node_user_t user, fs_node_group_t group,
                         fs_node_read_t read, fs_node_write_t write, fs_node_open_t open, fs_node_close_t close,
                         fs_node_finddir_t finddir, fs_node_readdir_t readdir, size_t len, uint32_t impl) {
    fs_node_t* node = KMALLOC(fs_node_t);
    strncpy(node->name, name, FS_FILENAME_MAX);
    node->perms = perms;
    node->flags = flags;
    node->user = user;
    node->group = group;
    node->read = read;
    node->write = write;
    node->open = open;
    node->close = close;
    node->finddir = finddir;
    node->readdir = readdir;
    node->len = len;
    node->impl = impl;
    node->seek = 0;
    return node;
}

size_t fs_read(fs_node_t *node, void *buff, size_t len, size_t offset) {
    if(node->read) return node->read(node, buff, len, offset);
    return 0;
}

size_t fs_write(fs_node_t *node, void *buff, size_t len, size_t offset) {
    if(node->write) return node->write(node, buff, len, offset);
    return 0;
}

bool fs_open(fs_node_t *node, char *mode) {
    if(node->open) return node->open(node, mode);
    return true;
}

bool fs_close(fs_node_t *node) {
    if(node->close) return node->close(node);
    return true;
}

fs_node_t *fs_readdir(fs_node_t *node, uint32_t idx) {
    if(node->readdir) return node->readdir(node, idx);
    return NULL;
}

fs_node_t *fs_finddir(fs_node_t *node, char *child_name) {
    if(node->finddir) return node->finddir(node, child_name);
    return NULL;
}
