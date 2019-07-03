#include "communication.h"

#include <time.h>
#include <mpi.h>
#include <stdio.h>

int send_safe_message(struct message *m) {
    MPI_Request r;
    MPI_Isend(m, sizeof(struct message), MPI_BYTE, m->id_t, 0, MPI_COMM_WORLD, &r);

    clock_t start = clock();
    clock_t last = start;
    struct timespec waiting_time;
    waiting_time.tv_nsec = 10000000;
    int flag = 0;
    //struct message m_ok;

    //MPI_Irecv(&m_ok, sizeof(struct message), MPI_BYTE, )

    while ((start - last) / (double) CLOCKS_PER_SEC * 1000 < TIMEOUT) {
        printf("waiting for %d\n", m->id_t);
        fflush(0);

        MPI_Test(&r, &flag, MPI_STATUS_IGNORE);
        if (flag) {
            printf("SUCCESS\n");
            return 1;
        }
        nanosleep(&waiting_time, NULL);
        waiting_time.tv_nsec *= 2;
        last = clock();
    }
    printf("node %d TIMEOUT\n", m->id_t);
    fflush(0);
    return 0;
}

void receive_safe_message(struct message *m)
{
    // TODO
    (void) m;
}
