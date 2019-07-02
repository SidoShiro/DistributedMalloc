#include "node.h"
#include "leader.h"
#include "globals.h"
#include "message.h"
#include "debug.h"

#include <mpi.h>

struct leader_resources *generate_leader_resources() {
    struct leader_resources *l_r = malloc(sizeof(struct leader_resources));
    l_r->leader_blks = NULL;
    l_r->leader_command_queue = NULL;
    return l_r;
}

void malloc_block() {

}

struct address_search *search_at_address(size_t address, struct leader_resources *l_r) {
    struct blocks *bs = l_r->leader_blks;
    for (size_t i = 0; i < bs->nb_blocks; i++) {
        struct block *b = bs->blks[i];
        if (b->address >= address && b->address + b->size < address) {
            struct address_search *add_s = malloc(sizeof(struct address_search));
            add_s->size = b->size;
            add_s->address = b->address;
            add_s->address_r = address;
            add_s->id = b->id;
            return add_s;
        }
    }
    return NULL;
}


void get_command(struct node *n, struct leader_resources *l_r, unsigned short user) {
    // int MPI_Irecv(void *buf, int count, MPI_Datatype datatype, int source,
    //              int tag, MPI_Comm comm, MPI_Request *request);
    void *buff = malloc(sizeof(struct message));
    int count = sizeof(struct message);
    MPI_Request r;
    MPI_Status st;
    MPI_Irecv(buff, count, MPI_CHAR, user, 0, MPI_COMM_WORLD, &r);

    if (0 == MPI_Wait(&r, &st)) {
        struct message *m = buff;
        struct command_queue *n_command = generate_command_queue(m->op, NULL);
        switch (m->op) {
            case OP_OK:
                debug("Leader recv OP OK from User", n->id);
                break;
            case OP_WRITE:
                debug("Leader recv OP WRITE from User", n->id);
                n_command->command = m->op;
                n_command->data = NULL;
                struct data_write *d_w = generate_data_write(m->size, m->size, NULL);
                void *wbuff = malloc(sizeof(char) * (m->size + 1));
                debug("Leader wait DATA from User for OP WRITE", n->id);
                MPI_Irecv(wbuff, m->size * sizeof(char), MPI_CHAR, user, 0, MPI_COMM_WORLD, &r);
                if (0 == MPI_Wait(&r, &st)) {
                    d_w->data = wbuff;
                    n_command->data = d_w;
                }
                debug("Leader recv DATA from User for OP WRITE : ", n->id);
                debug_n(d_w->data, n->id, d_w->size);
                l_r->leader_command_queue = push_command(l_r->leader_command_queue, n_command);
                break;
            case OP_READ:
                debug("Leader recv OP READ from User", n->id);
                n_command->command = m->op;
                n_command->data = NULL;
                struct data_read *d_r = generate_data_read(m->address, m->size);
                n_command->data = d_r;
                l_r->leader_command_queue = push_command(l_r->leader_command_queue, n_command);
                break;
            case OP_MALLOC:
                debug("Leader recv OP MALLOC from User", n->id);
                n_command->command = m->op;
                n_command->data = NULL;
                struct data_size *d_s = generate_data_size(m->size);
                n_command->data = d_s;
                l_r->leader_command_queue = push_command(l_r->leader_command_queue, n_command);
                break;
            default:
                debug("Leader recv ANY OP from User", n->id);
                break;
        }
    }
    free(buff);
}

void execute_malloc(struct node *n, struct leader_resources *l_r) {
    (void) n;
    (void) l_r;
}

void execute_read(struct node *n, struct leader_resources *l_r) {
    (void) n;
    (void) l_r;
}

void execute_write(struct node *n, struct leader_resources *l_r) {
    // MPI_Request r;
    // MPI_Status st;
    struct data_write *d_w;
    d_w = peek_command(l_r->leader_command_queue);
    (void) d_w;
    (void) n;
    (void) l_r;
    // TODO
    // struct message *m = generate_message(n->id, )
    // MPI_Isend()
}

void execute_command(struct node *n, struct leader_resources *l_r) {
    if (peek_user_command(l_r->leader_command_queue) != OP_NONE) {

        switch (peek_user_command(l_r->leader_command_queue)) {
            case OP_MALLOC:
                execute_malloc(n, l_r);
                break;
            case OP_FREE:
                break;
            case OP_WRITE:
                execute_write(n, l_r);
                break;
            case OP_READ:
                execute_read(n, l_r);
                break;
            case OP_DUMP:
                break;
            case OP_SNAP:
                break;
            case OP_KILL:
                break;
            case OP_REVIVE:
                break;
            case OP_NONE:
                break;
            case OP_OK:
                break;
            default:
                break;
        }
    }
}


void leader_loop(struct node *n, unsigned short terminal_id) {
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
        get_command(n, l_r, terminal_id);

        // execute_command(n, l_r);
        if (die == 1)
            break;
    }
}
