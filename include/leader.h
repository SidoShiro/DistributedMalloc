#ifndef DISTRIBUTEDMALLOC_LEADER_H
#define DISTRIBUTEDMALLOC_LEADER_H

#include <stdlib.h>

#include "node.h"
#include "message.h"
#include "command_queue.h"
#include "block.h"

struct leader_resources {
    struct blocks *leader_blks;
    struct command_queue *leader_command_queue;
};

struct address_search {
    unsigned short id;
    size_t r_address;
    size_t v_address;
    size_t n_address;
    size_t size;
};

void leader_loop(struct node *n, unsigned short terminal_id, unsigned short nb_nodes);

#endif /* !DISTRIBUTEDMALLOC_LEADER_H */
