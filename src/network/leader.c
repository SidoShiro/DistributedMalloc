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

void get_command(struct leader_resources *l_r, unsigned short user) {
    // int MPI_Irecv(void *buf, int count, MPI_Datatype datatype, int source,
    //              int tag, MPI_Comm comm, MPI_Request *request);
    void *buff = malloc(sizeof(struct message));
    int count = sizeof(struct message);
    MPI_Request r;
    MPI_Status st;
    MPI_Irecv(buff, count, MPI_CHAR, user, 0, MPI_COMM_WORLD, &r);

    if (0 == MPI_Wait(&r, &st)) {

        struct message *m = buff;
        struct command_queue *n_command = malloc(sizeof(struct command_queue));
        switch (m->op) {
            case OP_OK:
                break;
            case OP_WRITE:
                n_command->command = USER_OP_WRITE;
                n_command->data = NULL;
                struct data_write *d_w = = malloc(sizeof(struct data_write));
                void *wbuff = malloc(sizeof(char) * m->size);
                MPI_Irecv(wbuff, m->size, MPI_CHAR, user, 0, MPI_COMM_WORLD, &r);
                if (0 == MPI_Wait(&r, &st)) {
                    d_w->data = wbuff;
                    d_w->size = m->size;
                    d_w->address = m->id_o;
                    n_command->data = d_w;
                }
                break;
            case OP_READ:
                n_command->command = USER_OP_WRITE;
                n_command->data = NULL;
                struct data_write *d_r = malloc(sizeof(struct data_read));
                d_r->size = m->size;
                d_r->address = m->address;
                n_command->data = d_r;
                break;
            default:
                break;
        }
        l_r->leader_command_queue = push_command(l_r->leader_command_queue, n_command);
    }
}

void execute_command(struct leader_resources *l_r);


void leader_loop(unsigned short terminal_id) {
    // Init leader resource
    struct leader_resources *l_r = generate_leader_resources();

    /*
    // MPI_Isend();
    int MPI_Isend(const void *buf, int count, MPI_Datatype datatype, int dest, int tag,
                  MPI_Comm comm, MPI_Request *request)
    // MPI_Irecv();
    // MPI_Wait();
    // MPI_Test();

    int MPI_Irecv(void *buf, int count, MPI_Datatype datatype, int source,
                  int tag, MPI_Comm comm, MPI_Request *request);
    */
    int die = -1;
    while (1) {
        get_command(l_r, terminal_id);

        execute_command(l_r);

        if (die == 1)
            break;
    }
}
