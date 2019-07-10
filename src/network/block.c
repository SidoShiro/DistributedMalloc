#include "block.h"


struct block_register *generate_blocks(size_t nb_blks) {
    struct block_register *blks = malloc(sizeof(struct block_register));
    blks->blks = malloc((34 + nb_blks) * sizeof(struct block *));
    blks->nb_blocks = nb_blks;
    return blks;
}

struct block *generate_block(unsigned short id, size_t size, size_t node_address,
                             size_t virtual_address, char free) {
    struct block *blk = malloc(64 + sizeof(struct block));
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

void merge_free_block(struct block_register *blks) {
    for (size_t i = 0; i < blks->nb_blocks; i++) {
        struct block *b = blks->blks[i];
        while (b && b->next) {
            if (b->free == 0 && b->next->free == 0) {
                b->size = b->size + b->next->size;
                b->next = b->next->next;
            } else {
                b = b->next;
            }
        }
    }
}

struct block_register *init_nodes_same_size(unsigned short nb_nodes, size_t size) {
    struct block_register *blks = generate_blocks(nb_nodes);
    size_t v_address = 0;
    for (size_t i = 0; i < blks->nb_blocks; i++) {
        // Skip 0 (user), so from 1 to nb_nodes + 1
        blks->blks[i] = generate_block(i + 1, size, 0, v_address, 0);
        v_address += size;
    }
    printf("> /!\\ Global init has memory size of %zu bytes\n", v_address);
    return blks;
}

struct block *split_block_u(struct block *b, size_t size) {
    if (!b) {
        printf("FATAL ALLOCATION ERROR\n");
        return NULL;
    }
    if (b->size > size) {
        size_t su = b->size - size;
        b->size = size;
        struct block *b_next = b->next;
        add_block(b, b->id, su, b->node_address + size, b->virtual_address + size, 0);
        b->next->next = b_next;
        return b;
    }
    return NULL;
}

struct allocation_register *generate_allocs(size_t size_alloc) {
    struct allocation_register *a = malloc(sizeof(struct allocation_register));
    a->size_alloc = size_alloc;
    a->count_alloc = 0;
    a->allocs = malloc(sizeof(struct allocation) * size_alloc);
    return a;
}

void add_allocation(struct allocation_register *a_r, struct allocation *a) {
    for (size_t i = 0; i < a_r->count_alloc; i++)
        if (a_r->allocs[i] == NULL) {
            a_r->allocs[i] = a;
            return;
        }
    if (a_r->count_alloc >= a_r->size_alloc) {
        a_r->allocs[a_r->count_alloc] = a;
    } else {
        // x2 size
        a_r->size_alloc = a_r->size_alloc * 2;
        a_r->allocs = realloc(a_r->allocs, a_r->size_alloc * sizeof(struct allocation));

    }
    a_r->allocs[a_r->count_alloc] = a;
    a_r->count_alloc++;
}
