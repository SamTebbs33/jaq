//
// Created by Samuel Tebbs on 17/04/2018.
//

#include <initrd.h>
#include <mem.h>
#include <string.h>
#include <fs.h>

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

fs_node_t *initrd_init(uint32_t initrd_addr) {
    start_addr = initrd_addr;
    initrd = (initrd_t *) initrd_addr;
    nodes = KMALLOC_N(fs_node_t*, initrd->n_nodes);
    files = (initrd_file_t *) (initrd_addr + sizeof(initrd_t));
    for (uint32_t i = 0; i < initrd->n_nodes; ++i) {
        nodes[i] = fs_make_node(files[i].name, FS_PERMS_OWNER(FS_PERMS_READ | FS_PERMS_EXECUTE), 0, 0, 0, initrd_read, NULL, NULL, NULL, NULL, NULL, files[i].length, i);
    }

    root = fs_make_node("initrd", FS_PERMS_OWNER(FS_PERMS_READ | FS_PERMS_EXECUTE) | FS_PERMS_GROUP(FS_PERMS_READ) | FS_PERMS_ALL(FS_PERMS_READ), FS_FLAGS_DIR, 0, 0, NULL, NULL, NULL, NULL, initrd_finddir, initrd_readdir, 0, 0);
    return root;
}
