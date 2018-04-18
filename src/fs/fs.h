//
// Created by Samuel Tebbs on 17/04/2018.
//

#ifndef JAQ_FS_H
#define JAQ_FS_H

#include <stdinc.h>

#define FS_FILENAME_MAX 128

#define FS_PERMS_EXECUTE 0x1
#define FS_PERMS_WRITE 0x2
#define FS_PERMS_READ 0x4
#define FS_PERMS_ALL(perm) perm
#define FS_PERMS_GROUP(perm) perm << 3
#define FS_PERMS_OWNER(perm) perm << 6

#define FS_FLAGS_DIR 0x1

typedef uint16_t fs_node_perms_t;
typedef uint8_t fs_node_flags_t;
typedef uint32_t fs_node_user_t;
typedef uint32_t fs_node_group_t;

struct fs_node;

typedef size_t (*fs_node_read_t)(struct fs_node* node, void* buff, size_t len, size_t offset);
typedef size_t (*fs_node_write_t)(struct fs_node* node, void* buff, size_t len, size_t offset);
typedef bool (*fs_node_open_t)(struct fs_node* node, char* mode);
typedef bool (*fs_node_close_t)(struct fs_node* node);
typedef struct fs_node* (*fs_node_readdir_t)(struct fs_node* dir_node, uint32_t idx);
typedef struct fs_node* (*fs_node_finddir_t)(struct fs_node* dir_node, char* child_name);

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

    size_t len;
    uint32_t impl; // Implementation-specific
    uint32_t seek;
} fs_node_t;

fs_node_t* fs_make_node(char* name, fs_node_perms_t perms, fs_node_flags_t flags, fs_node_user_t user, fs_node_group_t group,
    fs_node_read_t read, fs_node_write_t write, fs_node_open_t open, fs_node_close_t close, fs_node_finddir_t finddir, fs_node_readdir_t readdir, size_t len, uint32_t impl);
size_t fs_read(fs_node_t* node, void* buff, size_t len, size_t offset);
size_t fs_write(fs_node_t* node, void* buff, size_t len, size_t offset);
bool fs_open(fs_node_t* node, char* mode);
bool fs_close(fs_node_t* node);
fs_node_t* fs_readdir(fs_node_t* node, uint32_t idx);
fs_node_t* fs_finddir(fs_node_t* node, char* child_name);

#endif //JAQ_FS_H
