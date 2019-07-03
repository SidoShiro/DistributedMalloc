#ifndef DISTRIBUTEDMALLOC_BLOCK_H
#define DISTRIBUTEDMALLOC_BLOCK_H

#include <stdlib.h>
#include <stdio.h>

/**
 * size of block
 * address on id node
 * address virtual (user view)
 * next block
 * free : 0 is free, 1 is allocated
 */
struct block {
    unsigned short id;
    size_t size;
    size_t node_address;
    size_t virtual_address;
    struct block *next;
    char free;
};

struct blocks {
    size_t nb_blocks;
    struct block **blks;
};

struct blocks *init_nodes_same_size(unsigned short nb_nodes, size_t size);

struct blocks *generate_blocks(size_t nb_blks);

struct block *generate_block(unsigned short id, size_t size, size_t node_address, size_t virtual_address, char free);

void
add_block(struct block *blk, unsigned short id, size_t size, size_t node_address, size_t virtual_address, char free);

void merge_free_block(struct blocks *blks);

struct blocks *init_nodes_same_size(unsigned short nb_nodes, size_t size);

#endif /* !DISTRIBUTEDMALLOC_BLOCK_H */
