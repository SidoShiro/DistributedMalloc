#include <communication.h>
#include <string.h>
#include <mpi.h>
#include <debug.h>
#include <leader_election.h>
#include "node.h"

struct node *generate_node(unsigned short id, size_t size) {
    struct node *n = malloc(sizeof(struct node));
    if (!n)
        return NULL;
    n->id = id;
    n->size = size;
    n->memory = malloc((1 + size) * sizeof(char));
    for (size_t i = 0; i < size; i++) {
        n->memory[i] = '#';
    }
    n->memory[size] = '\0';
    n->isleader = 0;
    n->is_dead = 0;
    return n;
}

void write_on_node(struct node *n, size_t address, char *data, size_t size) {
    if (address + size <= n->size) {
        void *mem_op_ptr = (n->memory + address);
        memcpy((void *) mem_op_ptr, (void *) data, size);
        debug("Write done", n->id);
        // debug_n((char *) n->memory, n->id, n->size);
    } else {
        // printf("aske_addr %zu, ask_mem %zu, size_mem %zu\n\n", address, size, n->size);
        debug("OP WRITE FAILED", n->id);
    }
}

/**
 *
 * @param n
 * @param address
 * @param data
 * @param size
 */
void read_on_node(struct node *n, size_t address, char *data, size_t size) {
    if (address + size <= n->size) {
        void *mem_op_ptr = (n->memory + address);
        memcpy((void *) data, (void *) mem_op_ptr, size);
    } else {
        debug("OP READ FAILED", n->id);
    }
}

void wait_if_dead(struct node *n) {
    while (n->is_dead) {
        debug("WAITING FOR REVIVE", n->id);
        struct message m;
        MPI_Recv(&m, sizeof(struct message), MPI_BYTE, MPI_ANY_SOURCE, TAG_REVIVE, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        if (m.op == OP_REVIVE) {
            debug("REVIVE __", n->id);
            n->is_dead = 0;
            int f = 0;
            MPI_Request request;
            do {
                struct message bin;
                MPI_Irecv(&bin, sizeof(struct message), MPI_BYTE, MPI_ANY_SOURCE, TAG_MSG, MPI_COMM_WORLD, &request);
                MPI_Test(&request, &f, MPI_STATUS_IGNORE);
            } while (f);
            MPI_Cancel(&request);
            f = 0;
            do {
                struct message bin;
                MPI_Irecv(&bin, sizeof(struct message), MPI_BYTE, MPI_ANY_SOURCE, TAG_ELECTION, MPI_COMM_WORLD, &request);
                MPI_Test(&request, &f, MPI_STATUS_IGNORE);
            } while (f);
            MPI_Cancel(&request);
        }
    }
}

void node_cycle(struct node *n) {
    struct queue *q = queue_init();
    while (1) {
        wait_if_dead(n);
        // cycle of node
        struct message *m = receive_message(q, TAG_MSG);
        //debug("Recv OP", n->id);
        switch (m->op) {
            case OP_START_LEADER:
                queue_free(q);
                free(m);
                debug("START LEADER ELECTION", n->id);
                return;
            case OP_OK:
                break;
            case OP_WRITE: {
                debug("Write OP : send OK", n->id);
                struct message *mW = generate_message(n->id, m->id_s, n->id, 0, 0, OP_OK);
                MPI_Send(mW, sizeof(struct message), MPI_BYTE, mW->id_t, TAG_MSG, MPI_COMM_WORLD);
                size_t addr = m->address;
                size_t size = m->size;
                char *data = malloc(size * sizeof(char));
                MPI_Status st3;
                MPI_Recv(data, size, MPI_BYTE, m->id_s, TAG_DATA, MPI_COMM_WORLD, &st3);
                write_on_node(n, addr, data, size);
                free(data);
            }
                break;
            case OP_READ:
                debug("Read OP", n->id);
                char *data = malloc(m->size * sizeof(char));
                read_on_node(n, m->address, data, m->size);
                debug("Send Read: Data", n->id);
                MPI_Send(data, m->size, MPI_BYTE, m->id_s, TAG_DATA, MPI_COMM_WORLD);
                free(data);
                break;
            case OP_KILL:
                debug("KILLED __", n->id);
                n->is_dead = 1;
                break;
            default:
                break;
        }
        free(m);
    } // while(1)
}
