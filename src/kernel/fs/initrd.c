//
// Created by Samuel Tebbs on 17/04/2018.
//

#include <fs/initrd.h>
#include <mem/mem.h>
#include <lib/string.h>
#include <fs/fs.h>
#include <log/log.h>

initrd_t* initrd;
fs_node_t* root;
fs_node_t** nodes;
initrd_file_t* files;
uint32_t start_addr;

size_t initrd_read(fs_node_t* node, void* buff, size_t len, size_t offset) {
    initrd_file_t file = files[node->impl];
    if(offset >= file.length) return 0;
    if(offset + len > file.length) len = file.length - offset;
    memcpy(buff, (void *) (start_addr + file.offset + offset), len);
    return len;
}

fs_node_t* initrd_readdir(fs_node_t* node, uint32_t idx) {
    if(node == root && idx < initrd->n_nodes) return nodes[idx];
    return NULL;
}

fs_node_t* initrd_finddir(fs_node_t* node, char* child_name) {
    if(node == root) {
        for (uint32_t i = 0; i < initrd->n_nodes; ++i) {
            if(!strcmp(child_name, nodes[i]->name)) return nodes[i];
        }
    }
    return NULL;
}

void initrd_init(uint32_t initrd_addr) {
    start_addr = initrd_addr;
    initrd = (initrd_t *) initrd_addr;
    fs_create_file(fs_make_dir_node("initrd", 0, FS_FLAGS_DIR, 0, 0, NULL, NULL, initrd_finddir, initrd_readdir, 0, 0), "/");
    nodes = KMALLOC_N(fs_node_t*, initrd->n_nodes);
    files = (initrd_file_t *) (initrd_addr + sizeof(initrd_t));
    // This loop goes through the initrd's files, creating any parent directories needed, and adds them to the VFS
    for (uint32_t i = 0; i < initrd->n_nodes; ++i) {
        char* path = files[i].name;
        char* path_dup = strdup(path);
        char* strtok_save = NULL;
        char* tok = strtok_r(path_dup, FS_PATH_SEP_STR, &strtok_save);
        // The path to the file being processed
        char path_buff[INITRD_FILENAME_MAX] = "";
        // The path to the file's parent, so we know what dir to create the file under
        char parent_buff[INITRD_FILENAME_MAX] = "";
        while(tok) {
            strcat(path_buff, "/");
            strcat(parent_buff, "/");
            strcat(path_buff, tok);
            bool is_file = strlen(path_buff) >= strlen(path);
            // If the file/directory doesn't exist then create it
            if(!fs_walk_path(path_buff)) {
                fs_node_t* node = is_file ? fs_make_file_node(tok, 0, 0, 0, 0, initrd_read, NULL, NULL, NULL, files[i].length, i) : fs_make_dir_node(tok, 0, 0, 0, 0, NULL, NULL, NULL, NULL, 0, 0);
                // If it is a file then add to the array of files that can be read from
                if(is_file) nodes[i] = node;
                // Add it to the VFS hierarchy
                fs_create_file(node, parent_buff);
            }
            strcat(parent_buff, tok);
            tok = strtok_r(NULL, FS_PATH_SEP_STR, &strtok_save);
        }
        kfree(path_dup);
    }
}
