//
// Created by Samuel Tebbs on 17/04/2018.
//

#ifndef JAQ_FS_H
#define JAQ_FS_H

#include "../kernel/stdint.h"
#include "../kernel/stdbool.h"

#define FS_FILENAME_MAX 128

typedef uint16_t fs_node_perms_t;
typedef uint8_t fs_node_flags_t;
typedef uint32_t fs_node_user_t;
typedef uint32_t fs_node_group_t;

struct fs_node;

typedef size_t (*fs_node_read_t)(void* buff, size_t len, size_t offset);
typedef size_t (*fs_node_write_t)(void* buff, size_t len, size_t offset);
typedef bool (*fs_node_open_t)(struct fs_node* node, char* mode);
typedef bool (*fs_node_close_t)(struct fs_node* node);
typedef struct fs_node (*fs_node_readdir_t)(struct fs_node* dir_node, uint32_t idx);
typedef struct fs_node (*fs_node_finddir_t)(struct fs_node* dir_node, char* child_name);

typedef struct fs_node {
    char name[FS_FILENAME_MAX];

    fs_node_perms_t perms;
    fs_node_flags_t flags;

    fs_node_user_t user;
    fs_node_group_t group;

    fs_node_read_t read;
    fs_node_write_t write;
    fs_node_open_t open;
    fs_node_close_t close;
    fs_node_finddir_t finddir;
    fs_node_readdir_t readdir;
} fs_node_t;

#endif //JAQ_FS_H
