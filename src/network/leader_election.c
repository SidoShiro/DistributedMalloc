#include "cli.h"
#include "graph.h"
#include "utils.h"
#include "debug.h"
#include "message.h"

#include "leader_election.h"

unsigned leader_election(unsigned id, unsigned network_size) {

    unsigned new_leader = id;
    unsigned leader = id;
    unsigned next = next_id(id, network_size);
    struct message *m = NULL;

    while (1) {
        if (new_leader <= leader) {
            leader = new_leader;
            m = generate_message(id, next, id, leader, 0, OP_LEADER);
            MPI_Send(m, sizeof(*m), MPI_BYTE, next, 201, MPI_COMM_WORLD);
        
            // free(m); ?
        }

        MPI_Recv(m, sizeof(*m), MPI_BYTE, MPI_ANY_SOURCE, 201, MPI_COMM_WORLD,
            MPI_STATUS_IGNORE);
        if (m->op == OP_OK)
            break; // Should be replaced by a OP_LEADER_OK
        if (m->op == OP_LEADER && m->id_o == id) {
            // send OK
            m = generate_message(id, 666, id, leader, 0, OP_OK); // does it really needs to be in the loop ?
            for (unsigned i = 1; i < network_size; ++i) {
                MPI_Send(m, sizeof(*m), MPI_BYTE, i, 201, MPI_COMM_WORLD);
            }
            // free(m);

            m = generate_message(id, 666, id, leader, 0, OP_OK);
            MPI_Send(m, sizeof(*m), MPI_BYTE, 0, 201, MPI_COMM_WORLD);
            break;
        }
    }

    return leader;
}


unsigned next_id(unsigned id, unsigned modulo) {
    unsigned res = (id  + 1) % modulo;
    if (res == 0)
        res = 1;
    return res;
}
