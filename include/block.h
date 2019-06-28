#ifndef DISTRIBUTEDMALLOC_BLOCK_H
#define DISTRIBUTEDMALLOC_BLOCK_H

#include "stdlib.h"

struct block {
    unsigned short id;
    size_t size;
    size_t address;
    struct block *next;
};

struct blocks {
    size_t nb_blocks;
    struct block **blks;
};

struct blocks *generate_blocks(size_t nb_blks);

struct block *generate_block(unsigned short id, size_t size, size_t address);

void add_block(struct block blk, unsigned short id, size_t size, size_t address);

#endif /* !DISTRIBUTEDMALLOC_BLOCK_H */
