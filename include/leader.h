#ifndef DISTRIBUTEDMALLOC_LEADER_H
#define DISTRIBUTEDMALLOC_LEADER_H

#include <stdlib.h>

#include "node.h"
#include "message.h"
#include "command_queue.h"
#include "block.h"

struct leader_resources {
    struct block_register *leader_blks;
    struct allocation_register  *leader_reg;
    struct command_queue *leader_command_queue;
    unsigned short id;
};

struct address_search {
    unsigned short id;
    size_t r_address;
    size_t v_address;
    size_t n_address;
    size_t size;
};

struct leader_resources *generate_leader_resources(size_t nb_nodes , size_t id);

void leader_loop(struct node *n, unsigned short terminal_id, unsigned short nb_nodes);

#endif /* !DISTRIBUTEDMALLOC_LEADER_H */
