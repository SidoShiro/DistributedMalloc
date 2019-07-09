#include <communication.h>
#include <string.h>
#include <mpi.h>
#include <debug.h>
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
    return n;
}

void write_on_node(struct node *n, size_t address, char *data, size_t size) {
    if (address + size <= n->size) {
        void *mem_op_ptr = (n->memory + address);
        memcpy((void *) mem_op_ptr, (void *) data, size);
        debug("Write done of :", n->id);
        debug_n((char *) n->memory, n->id, n->size);
    }
    else {
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
    }
    else {
        debug("OP READ FAILED", n->id);
    }
}

void node_cycle(struct node *n) {
    while (1) {
        // cycle of node
        // OLD FIXME struct queue *q = queue_init();
        struct message *m = generate_message(0, 0, 0, 0, 0, OP_NONE);
        MPI_Status st;
        MPI_Recv(m, sizeof(struct message), MPI_BYTE, MPI_ANY_SOURCE, 3, MPI_COMM_WORLD, &st);
        debug("Recv OP", n->id);
        switch (m->op) {
            case OP_OK:
                break;
            case OP_WRITE: {
                debug("Write OP : send OK", n->id);
                struct message *mW = generate_message(n->id, m->id_s, n->id, 0, 0, OP_OK);
                MPI_Send(mW, sizeof(struct message), MPI_BYTE, mW->id_t, 3, MPI_COMM_WORLD);
                size_t addr = m->address;
                size_t size = m->size;
                char *data = malloc(size * sizeof(char));
                MPI_Status st3;
                MPI_Recv(data, size, MPI_BYTE, m->id_s, 4, MPI_COMM_WORLD, &st3);
                write_on_node(n, addr, data, size);
                free(data);
            }
                break;
            case OP_READ:
                debug("Read OP", n->id);
                char *data = malloc(m->size * sizeof(char));
                read_on_node(n, m->address, data, m->size);
                debug("Send Read: Data", n->id);
                MPI_Send(data, m->size, MPI_BYTE, m->id_s, 4, MPI_COMM_WORLD);
                free(data);
                break;
            default:
                break;
        }
        if (m->op == OP_KILL)
            break;
        free(m);
    }
}
