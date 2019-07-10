#ifndef DISTRIBUTEDMALLOC_LEADER_H
#define DISTRIBUTEDMALLOC_LEADER_H

#include <communication.h>
#include <mpi.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include "block.h"
#include "command_queue.h"
#include "debug.h"
#include "globals.h"
#include "leader.h"
#include "message.h"
#include "node.h"
#include "utils.h"

struct dead_list {
    size_t max;
    unsigned short *dead_list;
};

struct leader_resources {
    struct block_register *leader_blks;
    struct allocation_register  *leader_reg;
    struct command_queue *leader_command_queue;
    unsigned short id;
    size_t availaible_memory;
    size_t max_memory;
    struct dead_list *dead_list;
};

struct leader_resources *generate_leader_resources(size_t nb_nodes , size_t id);

void leader_loop(struct node *n, unsigned short terminal_id, unsigned short nb_nodes);

struct allocation *give_for_v_address(struct leader_resources *l_r, size_t v_address, size_t *part);

size_t size_of_allocation(struct allocation *a);

void add_dead(struct leader_resources *l_r, unsigned short id);

void rm_dead(struct leader_resources *l_r, unsigned short id);

int is_in_dead_list(struct leader_resources *l_r, unsigned short id);

void print_allocations_table(struct leader_resources *l_r);

#endif /* !DISTRIBUTEDMALLOC_LEADER_H */
