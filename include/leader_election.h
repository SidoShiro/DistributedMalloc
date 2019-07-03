#ifndef DISTRIBUTEDMALLOC_LEADER_ELECTION_H
#define DISTRIBUTEDMALLOC_LEADER_ELECTION_H


#include <mpi.h>
#include <stdio.h>
#include "node.h"

unsigned leader_election(struct node* self, unsigned network_size);
unsigned next_id(unsigned id, unsigned modulo);

#endif