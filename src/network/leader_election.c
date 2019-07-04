#include "cli.h"
#include "graph.h"
#include "utils.h"
#include "debug.h"
#include "message.h"
#include "communication.h"

#include "leader_election.h"

unsigned leader_election(struct node* self, unsigned network_size) {

    unsigned id = self->id;
    unsigned new_leader = id;
    unsigned leader = id + 1;
    unsigned next = next_id(id, network_size);
    struct message *m = NULL;

    while (1) {
        if (new_leader < leader) {
            leader = new_leader;
            m = generate_message_a(id, next, leader, 0, 0, OP_LEADER, 1);
            // Old
            //MPI_Send(m, sizeof(*m), MPI_BYTE, next, 201, MPI_COMM_WORLD);
            // New_v2
            while (!send_safe_message(self, m)) {
                
                debug("Send timed out", id);
                // What if is was also m->id_o
                next = next_id(next, network_size);
                if (m->id_t == m->id_o) { // On devait dire au prochain qu'il était le leader
                    // FIXME: broadcast, restart
                }

                if (next == id) { // envoie à sois même => tous les autres sont morts .. fuck ?
                    // FIXME: envoyer un message d'erreur au user ?
                    break;
                }

                m->id_t = next;
            }
            if (next == id) {
                debug("Proclaming himself leader cause too many time-out", self->id);
                break;
            }
            // EndNew

            free(m);
        }

        // Old
        //MPI_Recv(m, sizeof(*m), MPI_BYTE, MPI_ANY_SOURCE, 201, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        // New
        if (self->pm != NULL && self->pm->m != NULL) {
            waiting_specific_op(self, m, OP_OK);
        }
        //receive_safe_message(m);

        if (m->op == OP_OK)
            break; // Should be replaced by a OP_LEADER_OK

        if (m->op == OP_LEADER) {
            new_leader = m->id_o;
            if (m->id_o == id) {
                // send OK
                m = generate_message_a(id, 666, leader, 0, 0, OP_OK, 0);
                for (unsigned i = 1; i < network_size; ++i) {
                    MPI_Send(m, sizeof(*m), MPI_BYTE, i, 201, MPI_COMM_WORLD); // FIXME : broadcast, need to be safe ?
                }
                free(m);

                // send leader to user
                m = generate_message_a(id, 666, leader, 0, 0, OP_OK, 0);
                MPI_Send(m, sizeof(*m), MPI_BYTE, 0, 201, MPI_COMM_WORLD);
                free(m);
                break;
            }
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
