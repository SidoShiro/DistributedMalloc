#include <communication.h>
#include <queue.h>
#include "cli.h"
#include "graph.h"
#include "utils.h"
#include "debug.h"
#include "message.h"

#include "leader_election.h"

unsigned leader_election(unsigned id, unsigned network_size) {
    unsigned new_leader = id;
    unsigned leader = id + 1;
    unsigned next = next_id(id, network_size);
    struct queue *message_queue = queue_init();

    while (1) {
        if (new_leader < leader) {
            //printf("%u changed its leader from %u to %u\n", id, leader, new_leader);
            leader = new_leader;
            struct message *m_send = generate_message_a(id, next, leader, 0, 0, OP_LEADER, 1);
            //printf("id:%u next:%u leader:%u op:%u\n", m_send->id_s, m_send->id_t, m_send->id_o, m_send->op);
            while (!send_safe_message(m_send, message_queue, TAG_ELECTION)) {
                debug("TIMEOUT", id);
                if (next == leader) {
                    debug("LEADER IS DEAD, STARTING AGAIN", id);
                    struct message *m_again = generate_message(id, 0, leader, 0, 0, OP_LEADER_AGAIN);
                    broadcast_message(m_again, id, network_size, TAG_ELECTION);
                    free(m_again);
                    free(m_send);
                    return leader_election(id, network_size); // TODO: Check if it works + fix leaks
                }
                next = next_id(next, network_size);
                if (next == id) {
                    debug("i'm alone", id);
                    queue_free(message_queue);
                    free(m_send);
                    return 0;
                }
                m_send->id_t = next;
                //printf("%u changed target to %u\n", id, next);
            }
            free(m_send);
        }
        //debug("safe message sent", id);
        struct message *m_receive = receive_message(message_queue, TAG_ELECTION);

        if (m_receive->op == OP_LEADER_OK) {
            queue_free(message_queue);
            free(m_receive);
            return leader;
        }

        if (m_receive->op == OP_LEADER_AGAIN) {
            queue_free(message_queue);
            free(m_receive);
            return leader_election(id, network_size); // TODO: Check if it works + fix leaks
        }

        if (m_receive->op == OP_LEADER) {
            new_leader = m_receive->id_o;
            if (m_receive->id_o == id) {
                // send LEADER OK
                struct message *m_ok = generate_message(id, 0, leader, 0, 0, OP_LEADER_OK);
                broadcast_message(m_ok, id, network_size, TAG_ELECTION);
                free(m_ok);

                // send leader to user
                struct message *m_user = generate_message(id, 0, leader, 0, 0, OP_OK);
                MPI_Send(m_user, sizeof(*m_user), MPI_BYTE, 0, TAG_MSG, MPI_COMM_WORLD);
                free(m_user);
                queue_free(message_queue);
                free(m_receive);
                return leader;
            }
        }
        free(m_receive);
    } // while(1)
}


unsigned next_id(unsigned id, unsigned modulo) {
    unsigned res = (id - 1) % modulo;
    if (res == 0)
        res = modulo - 1;
    return res;
}
