#include "cli.h"
#include "command_queue.h"
#include "message.h"
#include "globals.h"

#include <mpi.h>
#include <debug.h>

void send_write(void *data, unsigned short leader) {
    MPI_Request r;
    // MPI_Status st;
    struct data_write *d_w = data;
    struct message *m = generate_message(DEF_NODE_USER, leader, DEF_NODE_LEADER,
                                         d_w->address, d_w->size, OP_WRITE);

    MPI_Isend((void *) m, sizeof(struct message), MPI_BYTE, m->id_t, 0, MPI_COMM_WORLD, &r);

    debug("Send Data For Write OP", 0);
    MPI_Isend(d_w->data, d_w->size, MPI_BYTE, m->id_t, 0, MPI_COMM_WORLD, &r);
    free(m);
    free(data);
}

void send_malloc(void *data, unsigned short leader) {
    MPI_Request r;
    // MPI_Status st;
    struct data_size *d_s = data;
    struct message *m = generate_message(DEF_NODE_USER, leader, DEF_NODE_LEADER,
                                         0, d_s->size, OP_MALLOC);

    MPI_Isend((void *) m, sizeof(struct message), MPI_BYTE, m->id_t, 0, MPI_COMM_WORLD, &r);

    // TODO RECV Address !
    MPI_Status st;
    MPI_Request r2;
    void *buff = generate_message(0, 0, 0, 0, 0, OP_NONE);
    MPI_Irecv(buff, sizeof(struct message), MPI_BYTE, leader, 0, MPI_COMM_WORLD, &r2);
    while (0 != MPI_Wait(&r, &st)) {
        char *msg = "Address from malloc operation of size ";
        char size_str[256];
        snprintf(size_str, sizeof(size_str), "%zu", d_s->size);
        strcat(msg, size_str);
        debug(msg, DEF_NODE_USER);
    }
    free(m);
    free(data);
}

void send_read(void *data, unsigned short leader) {
    MPI_Request r;
    // MPI_Status st;
    struct data_read *d_r = data;
    struct message *m = generate_message(DEF_NODE_USER, leader, DEF_NODE_LEADER,
                                         d_r->address, d_r->size, OP_READ);

    MPI_Isend((void *) m, sizeof(struct message), MPI_CHAR, m->id_t, 0, MPI_COMM_WORLD, &r);

    // TODO RECV Read data !
    MPI_Status st;
    MPI_Request r2;
    void *buff = malloc(sizeof(char) * (d_r->size + 1));
    MPI_Irecv(buff, d_r->size * sizeof(char), MPI_CHAR, leader, 0, MPI_COMM_WORLD, &r2);
    while (0 != MPI_Wait(&r, &st)) {
        char *c_buff = buff;
        c_buff[d_r->size] = '\0';
        char *msg = "Read : ";
        strcat(msg, c_buff);
        debug(msg, DEF_NODE_USER);
    }
    free(m);
    free(data);
}

void send_command(enum operation op, void *data, unsigned short leader) {
    switch (op) {
        case OP_OK:
            break;
        case OP_MALLOC:
            debug("Send Malloc", 0);
            send_malloc(data, leader);
            break;
        case OP_WRITE:
            debug("Send Write", 0);
            send_write(data, leader);
            break;
        case OP_READ:
            debug("Send Read", 0);
            send_read(data, leader);
            break;
        default:
            break;
    }
}
