#include "block.h"


struct blocks *generate_blocks(size_t nb_blks) {
    struct blocks *blks = malloc(sizeof(struct blocks));
    blks->blks = malloc(nb_blks * sizeof(struct block*));
    blks->nb_blocks = nb_blks;
    return blks;
}

struct block *generate_block(unsigned short id, size_t size, size_t address) {
    struct block *blk = malloc(sizeof(struct block));
    blk->id = id;
    blk->size = size;
    blk->address = address;
    blk->next = NULL;
    return blk;
}

void add_block(struct block blk, unsigned short id, size_t size, size_t address) {
    struct block *n_blk = generate_block(id, size, address);
    blk.next = n_blk;
    (void) blk;
}
