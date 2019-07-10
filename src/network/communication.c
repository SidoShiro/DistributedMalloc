#include "communication.h"

#include <time.h>
#include <mpi.h>
#include <stdio.h>
#include <queue.h>
#include "debug.h"

int send_safe_message(struct message *m_send, struct queue *queue, int tag) {
    //debug("init send safe message", m_send->id_s);
    MPI_Send(m_send, sizeof(struct message), MPI_BYTE, m_send->id_t, tag, MPI_COMM_WORLD);

    struct message *m_recv = calloc(1, sizeof(struct message));
    MPI_Request r_ok;
    int f_ok = 0;
    MPI_Irecv(m_recv, sizeof(struct message), MPI_BYTE, MPI_ANY_SOURCE, tag, MPI_COMM_WORLD, &r_ok);

    struct timespec waiting_time;
    waiting_time.tv_sec = 0;
    waiting_time.tv_nsec = 100000000;

    for (int i = 0; i < NB_ITER; i++) {
        MPI_Test(&r_ok, &f_ok, MPI_STATUS_IGNORE);
        if (f_ok) {
            //debug("receive_something", m_recv->id_t);
            //printf("%u receive from %u, op = %u, leader = %u\n", m_recv->id_t, m_recv->id_s, m_recv->op, m_recv->id_o);
            if (m_recv->op == OP_ALIVE && m_recv->id_s == m_send->id_t) {
                free(m_recv); // free the last m_recv message which is not in queue
                return 1;
            }
            if (m_recv->need_callback) {
                struct message *m_alive = generate_message_a(m_recv->id_t, m_recv->id_s, 0, 0, 0, OP_ALIVE, 0);
                MPI_Send(m_alive, sizeof(struct message), MPI_BYTE, m_recv->id_s, tag, MPI_COMM_WORLD);
                free(m_alive);
            }
            queue_push_back(queue, m_recv);
            m_recv = calloc(1, sizeof(struct message));
            MPI_Irecv(m_recv, sizeof(struct message), MPI_BYTE, MPI_ANY_SOURCE, tag, MPI_COMM_WORLD, &r_ok);
        }
        nanosleep(&waiting_time, NULL);
    }
    MPI_Cancel(&r_ok);
    free(m_recv);
    return 0;
}

struct message *receive_message(struct queue *message_queue, int tag) {
    struct message *m_recv = queue_pop(message_queue);
    if (m_recv) {
        //debug("queue not empty", m_recv->id_t);
        return m_recv;
    }
    m_recv = calloc(1, sizeof(struct message));
    // What if m_recv->op = OP_ALIVE ?
    MPI_Recv(m_recv, sizeof(struct message), MPI_BYTE, MPI_ANY_SOURCE, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    //debug("queue empty, receive message", m_recv->id_t);
    if (m_recv->need_callback) {
        //debug("send callback", m_recv->id_t);
        struct message *m_alive = generate_message_a(m_recv->id_t, m_recv->id_s, 0, 0, 0, OP_ALIVE, 0);
        MPI_Send(m_alive, sizeof(struct message), MPI_BYTE, m_recv->id_s, tag, MPI_COMM_WORLD);
        //printf("%u respond to %u %u with a %u __ \n", m_alive->id_s, status.MPI_SOURCE, m_alive->id_t, m_alive->op);
        free(m_alive);
    }
    return m_recv;
}

void broadcast_message(struct message *m_send, unsigned id, unsigned network_size, int tag) {
    for (unsigned i = 1; i < network_size; ++i) {
        m_send->id_t = i;
        if (i != id)
            MPI_Send(m_send, sizeof(struct message), MPI_BYTE, i, tag, MPI_COMM_WORLD);
    }
}
