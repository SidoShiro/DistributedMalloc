#ifndef DISTRIBUTEDMALLOC_LEADER_ELECTION_H
#define DISTRIBUTEDMALLOC_LEADER_ELECTION_H


#include <mpi.h>
#include <stdio.h>


unsigned leader_election(unsigned id, unsigned network_size);
unsigned next_id(unsigned id, unsigned modulo);

#endif