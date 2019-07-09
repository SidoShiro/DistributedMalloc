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

struct block_register {
    size_t nb_blocks;
    struct block **blks;
};

struct allocation_register {
    size_t size_alloc;
    size_t count_alloc;
    struct allocation **allocs;
};

struct allocation {
    size_t number_parts;
    size_t v_address_start;
    struct block **parts;
};

struct allocation_register *generate_allocs(size_t size_alloc);

void add_allocation(struct allocation_register *a_r, struct allocation *a);

struct block_register *init_nodes_same_size(unsigned short nb_nodes, size_t size);

struct block_register *generate_blocks(size_t nb_blks);

struct block *generate_block(unsigned short id, size_t size, size_t node_address, size_t virtual_address, char free);

void
add_block(struct block *blk, unsigned short id, size_t size, size_t node_address, size_t virtual_address, char free);

void merge_free_block(struct block_register *blks);

/**
 * Splited half 2 is free = 0
 * @param b
 * @param size
 * @return
 */
struct block *split_block_u(struct block *b, size_t size);

#endif /* !DISTRIBUTEDMALLOC_BLOCK_H */
