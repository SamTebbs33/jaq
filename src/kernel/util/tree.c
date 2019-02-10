//
// Created by sam on 09/02/19.
//

#include <util/tree.h>
#include <mem/mem.h>

tree_t *tree_create(void *ptr) {
    tree_t *tree = kmalloc(sizeof(tree_t));
    if(tree) {
        tree->children = linkedlist_create();
        if(!tree->children) {
            kfree(tree);
            return false;
        }
        tree->ptr = ptr;
    }
    return tree;
}

bool tree_add(tree_t *tree, void *ptr) {
    return linkedlist_add(tree->children, tree_create(ptr));
}

bool tree_remove(tree_t *tree, uint32_t child) {
    if(linkedlist_size(tree->children) <= child) return false;
    tree_t* child_tree = linkedlist_get(tree->children, child);
    size_t num_grandchildren = linkedlist_size(child_tree->children);
    for (uint32_t i = 0; i < num_grandchildren; ++i) {
        tree_remove(child_tree, i);
    }
    kfree(child_tree);
    return true;
}

void *tree_get(tree_t *tree, uint32_t child) {
    return tree_get_child(tree, child)->ptr;
}

size_t tree_size(tree_t *tree) {
    return linkedlist_size(tree->children);
}

tree_t *tree_get_child(tree_t *tree, uint32_t child) {
    return linkedlist_get(tree->children, child);
}