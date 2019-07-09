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
    n->memory = malloc(size * sizeof(char) + 1);
    n->isleader = 0;
    return n;
}

void write_on_node(struct node *n, size_t address, char *data, size_t size) {
    if (address < n->size && address + size < n->size) {
        n->memory = (char *) memcpy((void *) (n->memory + address), (void *) data, size);
        debug("Write done of :", n->id);
        debug_n((char *)n->memory, n->id, n->size);
    }
}

void node_cycle(struct node *n) {
    // cycle of node
    struct message m;
    receive_message(&m);

    switch (m.op) {
        case OP_OK:
            break;
        case OP_MALLOC:
            break;
        case OP_FREE:
            break;
        case OP_WRITE: {
            size_t addr = m.address;
            size_t size = m.size;
            char *data = malloc(size * sizeof(char));
            MPI_Status st;
            MPI_Recv(data, size, MPI_BYTE, m.id_s, 4, MPI_COMM_WORLD, &st);
            write_on_node(n, addr, data, size);
            free(data);
        }
            break;
        case OP_READ:
            break;
        case OP_SNAP:
            break;
        case OP_LEADER:
            break;
        case OP_WHOISLEADER:
            break;
        case OP_REVIVE:
            break;
        case OP_KILL:
            break;
        case OP_TEST:
            break;
        case OP_NONE:
            break;
        case OP_DUMP:
            break;
        case OP_LEADER_OK:
            break;
        case OP_ALIVE:
            break;
        case OP_LEADER_AGAIN:
            break;
    }
}
