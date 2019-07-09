#include "communication.h"

#include <time.h>
#include <mpi.h>
#include <stdio.h>
#include <queue.h>
#include "debug.h"


void *recieve_data(size_t size, struct queue *queue, unsigned source) {

    MPI_Request r_data;
    void *data = calloc(1, size);
    MPI_Irecv(data, size, MPI_CHAR, source, TAG_DATA, MPI_COMM_WORLD, &r_data);

    MPI_Request r_msg;
    struct message *m_recv = calloc(1, sizeof(struct message));
    MPI_Irecv(m_recv, sizeof(struct message), MPI_CHAR, MPI_ANY_SOURCE, TAG_MSG, MPI_COMM_WORLD, &r_msg);

    struct timespec waiting_time;
    waiting_time.tv_sec = 0;
    waiting_time.tv_nsec = 100000000;

    int f_msg = 0;
    int f_data = 0;

    for (int i = 0; i < NB_ITER; i++) {
        // Checking messages recieved
        MPI_Test(&r_msg, &f_msg, MPI_STATUS_IGNORE);
        if (f_msg) {
            if (m_recv->need_callback) {
                struct message *m_alive = generate_message_a(m_recv->id_t, m_recv->id_s, 0, 0, 0, OP_ALIVE, 0);
                MPI_Send(m_alive, sizeof(struct message), MPI_BYTE, m_recv->id_s, TAG_MSG, MPI_COMM_WORLD);
                free(m_alive);
            }
            queue_push_back(queue, m_recv);
            m_recv = calloc(1, sizeof(struct message));
            MPI_Irecv(m_recv, sizeof(struct message), MPI_CHAR, MPI_ANY_SOURCE, TAG_MSG, MPI_COMM_WORLD, &r_msg);
        }
        // Checking data recieved
        MPI_Test(&r_data, &f_data, MPI_STATUS_IGNORE);
        if (f_data) {
            MPI_Cancel(&r_msg);
            return data;
        }
        nanosleep(&waiting_time, NULL);
    }
    MPI_Cancel(&r_msg);
    MPI_Cancel(&r_data);
    free(m_recv);
    return 0;

}