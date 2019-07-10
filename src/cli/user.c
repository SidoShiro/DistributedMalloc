#include "cli.h"
#include "command_queue.h"
#include "message.h"
#include "globals.h"

#include <debug.h>
#include <mpi.h>
#include <stdint.h>
#include <sys/types.h>
#include <unistd.h>
#include <communication.h>

void send_write(void *data, unsigned short leader) {
    // MPI_Status st;
    struct data_write *d_w = data;
    struct message *m = generate_message(DEF_NODE_USER, leader, DEF_NODE_LEADER,
                                         d_w->address, d_w->size, OP_WRITE);

    MPI_Send((void *) m, sizeof(struct message), MPI_BYTE, m->id_t, TAG_MSG, MPI_COMM_WORLD);

    debug("Send Data For Write OP", 0);
    MPI_Send(d_w->data, d_w->size, MPI_BYTE, m->id_t, TAG_DATA, MPI_COMM_WORLD);
    free(m);
}

void send_free(void *data, unsigned short leader) {
    // MPI_Status st;
    struct data_address *d_a = data;
    struct message *m = generate_message(DEF_NODE_USER, leader, DEF_NODE_LEADER,
                                         d_a->address, 0, OP_FREE);

    MPI_Send((void *) m, sizeof(struct message), MPI_BYTE, m->id_t, TAG_MSG, MPI_COMM_WORLD);
    free(m);
}

void send_table(unsigned short leader) {
    // MPI_Status st;
    struct message *m = generate_message(DEF_NODE_USER, leader, DEF_NODE_LEADER,
                                         0, 0, OP_TABLE);

    MPI_Send((void *) m, sizeof(struct message), MPI_BYTE, m->id_t, TAG_MSG, MPI_COMM_WORLD);
    free(m);
}

void send_malloc(void *data, unsigned short leader) {
    // MPI_Status st;
    struct data_size *d_s = data;
    struct message *m = generate_message(DEF_NODE_USER, leader, DEF_NODE_LEADER,
                                         0, d_s->size, OP_MALLOC);

    MPI_Send((void *) m, sizeof(struct message), MPI_BYTE, m->id_t, TAG_MSG, MPI_COMM_WORLD);

    // TODO RECV Address !
    MPI_Status st;
    void *buff = generate_message(0, 0, 0, 0, 0, OP_NONE);
    MPI_Recv(buff, sizeof(struct message), MPI_BYTE, leader, TAG_MSG, MPI_COMM_WORLD, &st);

    struct message *m2 = buff;
    if (m2->address != SIZE_MAX) {
        printf("user: request malloc of size %zu, is at address %zu on the network\n",
               m2->size,
               m2->address);
    } else if (m2->size == 0) {
        debug("Network: Out of memory", 0);
    } else {
        debug("Network: Fatal error in leader", 0);
    }

    free(m);
    free(buff);
}

void send_read(void *data, unsigned short leader) {
    // MPI_Status st;
    struct data_read *d_r = data;
    struct message *m = generate_message(DEF_NODE_USER, leader, DEF_NODE_LEADER,
                                         d_r->address, d_r->size, OP_READ);

    MPI_Send((void *) m, sizeof(struct message), MPI_BYTE, m->id_t, TAG_MSG, MPI_COMM_WORLD);

    struct message m2;
    MPI_Status st2;
    MPI_Recv(&m2, sizeof(struct message), MPI_BYTE, m->id_t, TAG_MSG, MPI_COMM_WORLD, &st2);
    debug("RECV OK with datasize read", 0);
    size_t real_data_size = m2.size;
    MPI_Status st3;
    char *r_data = malloc(sizeof(char) * (2 + real_data_size));
    MPI_Recv((void *) r_data, real_data_size * sizeof(char), MPI_BYTE, leader, TAG_DATA, MPI_COMM_WORLD, &st3);
    debug("Read:", 0);
    r_data[real_data_size + 1] = '\0';
    debug_n(r_data, 0, real_data_size);
    free(m);
}

void send_read_file(void *data, unsigned short leader) {
    MPI_Request r;
    // MPI_Status st;
    struct data_write *d_r = data;
    if (d_r->size == 0)
        d_r->size = 40000;

    struct message *m = generate_message(DEF_NODE_USER, leader, DEF_NODE_LEADER,
                                         d_r->address, d_r->size, OP_READ);

    MPI_Isend((void *) m, sizeof(struct message), MPI_BYTE, m->id_t, TAG_MSG, MPI_COMM_WORLD, &r);

    struct message *m2 = generate_message(DEF_NODE_USER, leader, DEF_NODE_LEADER,
                                          d_r->address, d_r->size, OP_READ);
    MPI_Status st2;
    MPI_Recv(m2, sizeof(struct message), MPI_BYTE, m->id_t, TAG_MSG, MPI_COMM_WORLD, &st2);
    debug("RECV OK with datasize read", 0);
    size_t real_data_size = m2->size;
    MPI_Status st3;
    d_r->data = malloc(sizeof(char) * (2 + real_data_size));
    d_r->size = real_data_size;
    debug("Read Recv Data", 0);
    MPI_Recv(d_r->data, real_data_size * sizeof(char), MPI_BYTE, leader, TAG_DATA, MPI_COMM_WORLD, &st3);
    printf("Data Read :: %zu", d_r->size);
    free(m);
    free(m2);
}

void send_dump(void *data, unsigned short leader) {
    // MPI_Status st;
    struct data_address *d_a = data;
    struct message *m = generate_message(DEF_NODE_USER, leader, DEF_NODE_LEADER,
                                         d_a->address, 0, OP_DUMP);

    MPI_Send((void *) m, sizeof(struct message), MPI_BYTE, m->id_t, TAG_MSG, MPI_COMM_WORLD);

    free(m);
}

void send_dump_all(unsigned short leader) {
    // MPI_Status st;
    struct message *m = generate_message(DEF_NODE_USER, leader, DEF_NODE_LEADER,
                                         0, 0, OP_DUMP_ALL);

    MPI_Send((void *) m, sizeof(struct message), MPI_BYTE, m->id_t, TAG_MSG, MPI_COMM_WORLD);

    free(m);
}

void send_command(enum operation op, void *data, unsigned short leader) {
    switch (op) {
        case OP_OK:
            break;
        case OP_MALLOC:
            debug("Send Malloc", 0);
            send_malloc(data, leader);
            break;
        case OP_FREE:
            debug("Send Free", 0);
            send_free(data, leader);
            break;
        case OP_TABLE:
            debug("Send Table", 0);
            send_table(leader);
            break;
        case OP_WRITE:
            debug("Send Write", 0);
            send_write(data, leader);
            break;
        case OP_READ:
            debug("Send Read", 0);
            send_read(data, leader);
            break;
        case OP_DUMP:
            debug("Send Dump", 0);
            send_dump(data, leader);
            break;
        case OP_DUMP_ALL:
            debug("Send Dump All", 0);
            send_dump_all(leader);
            break;
        case OP_READ_FILE:
            debug("Send Read (file)", 0);
            send_read_file(data, leader);
            break;
        default:
            break;
    }
}
