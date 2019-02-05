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

/**
 * A filesystem node. Can be virtual or correlate to physical storage
 */
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

extern fs_node_t* fs_root;

/**
 * Create a filesystem node with certain flags and functions associated with it
 * @param name
 * @param perms
 * @param flags
 * @param user
 * @param group
 * @param read
 * @param write
 * @param open
 * @param close
 * @param finddir
 * @param readdir
 * @param len
 * @param impl Don't remember what this is for, will update when I do...
 * @return
 */
fs_node_t* fs_make_node(char* name, fs_node_perms_t perms, fs_node_flags_t flags, fs_node_user_t user, fs_node_group_t group,
    fs_node_read_t read, fs_node_write_t write, fs_node_open_t open, fs_node_close_t close, fs_node_finddir_t finddir, fs_node_readdir_t readdir, size_t len, uint32_t impl);

/**
 * Read from an fs_node into a buffer. This will invoke the node's `read` function if given
 * @param node The node to read from
 * @param buff The byte buffer to read into
 * @param len The maximum length in bytes to read. Shouldn't overstep the size of the buffer
 * @param offset The offset into the file at which to start reading from
 * @return The number of bytes read. Should always be <= len
 */
size_t fs_read(fs_node_t* node, void* buff, size_t len, size_t offset);

/**
 * Write a buffer to an fs_node. This will invoke the node's `write` function if given
 * @param node The node to write to
 * @param buff The byte buffer to write
 * @param len The maximum length in bytes to write. Shouldn't overstep the size of the buffer
 * @param offset The offset into the file at which to start writing
 * @return The number of bytes witten. Should always be <= len
 */
size_t fs_write(fs_node_t* node, void* buff, size_t len, size_t offset);

/**
 * Open a node for reading/writing with a given mode
 * @param node The node to open
 * @param mode The mode. Can be one of the below options:
 *  "r": Open for reading. Future write attempts may fail
 *  "r+": Open for reading and writing
 *  "w": Create the node if it didn't exist, else clear it and open for writing
 *  "w+": Create the node if it didn't exist, else clear it and open for reading and writing
 *  "a": Create the node if it didn't exist and open for writing at the end of the node
 *  "a+": Create the node if it didn't exist, and open for writing from the end and reading from the start
 * @return True if opening succeeded else false
 */
bool fs_open(fs_node_t* node, char* mode);

/**
 * Close a node. Implementation-defined behaviour
 * @param node The node to close
 * @return True if closing succeeded, else false
 */
bool fs_close(fs_node_t* node);

/**
 * Find the entries within a directory.
 * @param node The directory to read from
 * @param idx The index into the directory's entries
 * @return The found node or NULL if there wasn't one at this index
 */
fs_node_t* fs_readdir(fs_node_t* node, uint32_t idx);

/**
 * Find a child node with a given path beneath the parent node.
 * @param node The parent node under which to search
 * @param child_name The path to the child node
 * @return The node or NULL if it doesn't exist
 */
fs_node_t* fs_finddir(fs_node_t* node, char* child_name);

#endif //JAQ_FS_H
