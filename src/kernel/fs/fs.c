//
// Created by Samuel Tebbs on 17/04/2018.
//

#include <fs/fs.h>
#include <mem/mem.h>
#include <util/string.h>
#include <util/tree.h>
#include <util/util.h>
#include <screen/print.h>

tree_t* fs_tree, * current_fs_tree;
fs_node_t* fs_root;

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

fs_node_t * fs_make_file_node(char *name, fs_node_perms_t perms, fs_node_flags_t flags, fs_node_user_t user, fs_node_group_t group,
                  fs_node_read_t read, fs_node_write_t write, fs_node_open_t open, fs_node_close_t close, size_t len,
                  uint32_t impl) {
    fs_node_t* node = kmalloc(sizeof(fs_node_t));
    strncpy(node->name, name, FS_FILENAME_MAX);
    node->perms = perms;
    node->flags = flags;
    node->user = user;
    node->group = group;
    node->read = read;
    node->write = write;
    node->open = open;
    node->close = close;
    node->len = len;
    node->impl = impl;
    return node;
}

fs_node_t * fs_make_dir_node(char *name, fs_node_perms_t perms, fs_node_flags_t flags, fs_node_user_t user, fs_node_group_t group,
                 fs_node_open_t open, fs_node_close_t close, fs_node_finddir_t finddir, fs_node_readdir_t readdir,
                 size_t len, uint32_t impl) {
    fs_node_t* node = kmalloc(sizeof(fs_node_t));
    strncpy(node->name, name, FS_FILENAME_MAX);
    node->perms = perms;
    node->flags = (fs_node_flags_t) (flags | FS_FLAGS_DIR);
    node->user = user;
    node->group = group;
    node->readdir = readdir;
    node->finddir = finddir;
    node->open = open;
    node->close = close;
    node->len = len;
    node->impl = impl;
    return node;
}

tree_t* fs_walk_tree_rec(char* segment, tree_t* tree) {
    for (uint32_t i = 0; i < tree_size(tree); ++i) {
        tree_t* child = tree_get_child(tree, i);
        char* child_name = (char*)&((fs_node_t*)child->ptr)->name;
        if(strcmp(segment, child_name) == 0) {
            // Get next path segment
            segment = strtok(NULL, FS_PATH_SEP_STR);
            if(segment) {
                // If the child isn't a directory then the file at the next segment doesn't exist
                if(!fs_is_dir(child->ptr)) return NULL;
                return fs_walk_tree_rec(segment, child);
            }
            else return child;
        }
    }
    return NULL;
}

bool fs_is_dir(fs_node_t *node) {
    return (node->flags & FS_FLAGS_DIR) != 0;
}

fs_node_t *fs_walk_path(char *path) {
    tree_t* tree = fs_walk_tree(path);
    return tree ? tree->ptr : NULL;
}

bool fs_create_file(fs_node_t *node, char *parent_path) {
    tree_t* parent = fs_walk_tree(parent_path);
    if(parent) return tree_add(parent, node);
    return false;
}

tree_t *fs_walk_tree(char *path) {
    tree_t* tree;
    // If the path starts with / then we should start at the root
    if(path[0] == FS_PATH_SEP) {
        tree = fs_tree;
        // Skip the /
        path++;
    } else tree = current_fs_tree;
    char* path_dup = strdup(path);
    char* segment = strtok(path_dup, FS_PATH_SEP_STR);
    if(segment) tree = fs_walk_tree_rec(segment, fs_tree);
    kfree(path_dup);
    return tree;
}

void fs_init() {
    fs_root = fs_make_dir_node("/", 0, FS_FLAGS_DIR, 0, 0, NULL, NULL, NULL, NULL, 0, 0);
    fs_tree = tree_create(fs_root);
}
