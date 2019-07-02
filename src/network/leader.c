#include "node.h"
#include "leader.h"
#include "globals.h"
#include <mpi.h>

struct leader_resources *generate_leader_resources() {
    struct leader_resources *l_r = malloc(sizeof(struct leader_resources));
    l_r->leader_blks = NULL;
    l_r->leader_command_queue = NULL;
    return l_r;
}


