//
// Created by sam on 10/02/19.
//

#ifndef JAQ_DEVFS_H
#define JAQ_DEVFS_H

#include <fs/fs.h>

extern fs_node_t* dev, * stdout, * stderr, * serial, * stdin;

void devfs_init();

#endif //JAQ_DEVFS_H
