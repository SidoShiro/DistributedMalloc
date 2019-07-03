#include "communication.h"
#include "node.h"
#include "pending_messages.h"

#include <time.h>
#include <mpi.h>
#include <stdio.h>

int send_safe_message(struct message *m_send) {
    MPI_Request r_send;
    int f_send = 0;
    MPI_Isend(m_send, sizeof(struct message), MPI_BYTE, m_send->id_t, 0, MPI_COMM_WORLD, &r_send);

    struct message m_ok;
    MPI_Request r_ok;
    int f_ok = 0;
    MPI_Irecv(&m_ok, sizeof(struct message), MPI_BYTE, m_send->id_t, 0, MPI_COMM_WORLD, &r_ok);

    struct timespec waiting_time;
    waiting_time.tv_sec = 0;
    waiting_time.tv_nsec = 10000000;

    for (int i = 0; i < NB_ITER; i++) {
        //printf("waiting for %d\n", m_send->id_t);
        fflush(0);

        if (!f_send) {
            MPI_Test(&r_send, &f_send, MPI_STATUS_IGNORE);
        } else {
            //printf("SEND OK\n");
            fflush(0);
            MPI_Test(&r_ok, &f_ok, MPI_STATUS_IGNORE);
            if (f_ok) {
                //printf("SUCCESS\n");
                fflush(0);
                return m_ok.op == OP_OK;
            }
        }
        //printf("start sleep\n");
        fflush(0);
        nanosleep(&waiting_time, NULL);
        //printf("end sleep\n");
        fflush(0);
        waiting_time.tv_nsec *= 2;
    }
    //printf("node %d TIMEOUT\n", m_send->id_t);
    fflush(0);
    return 0;
}

void receive_safe_message(struct message *m_recv) {
    MPI_Status status;
    MPI_Recv(m_recv, sizeof(struct message), MPI_BYTE, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
    //printf("RECEIVE SUCCESS\n");
    fflush(0);
    struct message *m_send = generate_message_a(m_recv->id_t, m_recv->id_s, 0, 0, 0, OP_OK, 0);
    MPI_Send(m_send, sizeof(struct message), MPI_BYTE, status.MPI_SOURCE, 0, MPI_COMM_WORLD);
    free(m_send);
    //printf("SEND SUCCESS\n");
}

void waiting_specific_op(struct node* self, struct message* m_recv, enum operation op) {
    
    while (1) {
        MPI_Status status;
        MPI_Recv(m_recv, sizeof(struct message), MPI_BYTE, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
        //printf("RECEIVE SUCCESS\n");
        if (m_recv->is_a) {
            struct message *m_send = generate_message_a(m_recv->id_t, m_recv->id_s, 0, 0, 0, OP_OK, 0);
            MPI_Send(m_send, sizeof(struct message), MPI_BYTE, status.MPI_SOURCE, 0, MPI_COMM_WORLD);
            free(m_send);
        }
        if (m_recv->op == op)
            break;
        else {
            pending_messages_add(self->pm, m_recv);
        }
    }
    //printf("SEND SUCCESS\n");
}
