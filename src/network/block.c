#include "block.h"


struct blocks *generate_blocks(size_t nb_blks) {
    struct blocks *blks = malloc(sizeof(struct blocks));
    blks->blks = malloc(nb_blks * sizeof(struct block*));
    blks->nb_blocks = nb_blks;
    return blks;
}

struct block *generate_block(unsigned short id, size_t size, size_t node_address,
        size_t virtual_address, char free) {
    struct block *blk = malloc(sizeof(struct block));
    blk->id = id;
    blk->size = size;
    blk->node_address = node_address;
    blk->virtual_address = virtual_address;
    blk->free = free;
    blk->next = NULL;
    return blk;
}

void add_block(struct block *blk, unsigned short id, size_t size, size_t node_address,
               size_t virtual_address, char free) {
    struct block *n_blk = generate_block(id, size, node_address, virtual_address, free);
    blk->next = n_blk;
    (void) blk;
}

void merge_free_block(struct blocks *blks);

struct blocks *init_nodes_same_size(unsigned short nb_nodes, size_t size) {
    struct blocks *blks = generate_blocks(nb_nodes);
    size_t  v_address = 0;
    for (size_t i = 0; i < blks->nb_blocks; i++) {
        // Skip 0 (user), so from 1 to nb_nodes + 1
        blks->blks[i] = generate_block(i + 1, size, 0, v_address, 0);
        v_address += size;
    }
    printf("Global init has memory size of %zu bytes", v_address);
    return blks;
}
