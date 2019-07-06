#include "communication.h"

#include <time.h>
#include <mpi.h>
#include <stdio.h>
#include <queue.h>
#include "debug.h"

int send_safe_message(struct message *m_send, struct queue *queue) {
    debug("init send safe message", m_send->id_s);
    MPI_Send(m_send, sizeof(struct message), MPI_BYTE, m_send->id_t, 201, MPI_COMM_WORLD);

    struct message *m_recv = calloc(1, sizeof(struct message));
    MPI_Request r_ok;
    int f_ok = 0;
    MPI_Irecv(m_recv, sizeof(struct message), MPI_BYTE, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &r_ok);

    struct timespec waiting_time;
    waiting_time.tv_sec = 0;
    waiting_time.tv_nsec = 100000000;

    for (int i = 0; i < NB_ITER; i++) {
        MPI_Test(&r_ok, &f_ok, MPI_STATUS_IGNORE);
        if (f_ok) {
            debug("receive_something", m_recv->id_t);
            if (m_recv->op == OP_ALIVE) {
                free(m_recv); // free the last m_recv message which is not in queue
                return 1;
            }
            if (m_recv->need_callback) {
                struct message *m_alive = generate_message_a(m_recv->id_t, m_recv->id_s, 0, 0, 0, OP_ALIVE, 0);
                MPI_Send(m_alive, sizeof(struct message), MPI_BYTE, m_recv->id_s, 201, MPI_COMM_WORLD);
                free(m_alive);
            }
            queue_push_back(queue, m_recv);
            m_recv = calloc(1, sizeof(struct message));
            MPI_Irecv(m_recv, sizeof(struct message), MPI_BYTE, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &r_ok);
        }
        nanosleep(&waiting_time, NULL);
    }
    return 0;
}

void receive_message(struct message *m_recv) {
    MPI_Status status;
    // What if m_recv->op = OP_ALIVE ?
    MPI_Recv(m_recv, sizeof(struct message), MPI_BYTE, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
    if (m_recv->need_callback) {
        debug("need callback", m_recv->id_t);
        struct message *m_send = generate_message_a(m_recv->id_t, m_recv->id_s, 0, 0, 0, OP_ALIVE, 0);
        MPI_Send(m_send, sizeof(struct message), MPI_BYTE, status.MPI_SOURCE, 201, MPI_COMM_WORLD);
        free(m_send);
    }
}
