#include "node.h"
#include "leader.h"
#include "globals.h"
// #include "debug.h"

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
    (void) n;
    // int MPI_Irecv(void *buf, int count, MPI_Datatype datatype, int source,
    //              int tag, MPI_Comm comm, MPI_Request *request);
    void *buff = malloc(sizeof(struct message));
    int count = sizeof(struct message);
    MPI_Request r;
    MPI_Status st;
    MPI_Irecv(buff, count, MPI_CHAR, user, 0, MPI_COMM_WORLD, &r);

    if (0 == MPI_Wait(&r, &st)) {
        // debug("Leader receive", n->id);
        struct message *m = buff;
        struct command_queue *n_command = malloc(sizeof(struct command_queue));
        switch (m->op) {
            case OP_OK:
                break;
            case OP_WRITE:
                n_command->command = USER_OP_WRITE;
                n_command->data = NULL;
                struct data_write *d_w = malloc(sizeof(struct data_write));
                void *wbuff = malloc(sizeof(char) * m->size);
                // debug("Leader wait data for OP WRITE from user", n->id);
                MPI_Irecv(wbuff, m->size, MPI_CHAR, user, 0, MPI_COMM_WORLD, &r);
                if (0 == MPI_Wait(&r, &st)) {
                    d_w->data = wbuff;
                    d_w->size = m->size;
                    d_w->address = m->id_o;
                    n_command->data = d_w;
                }
                // debug("Leader recv from User OP WRITE, completly", n->id);
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
    if (peek_user_command(l_r->leader_command_queue) != USER_OP_NONE) {

        switch (peek_user_command(l_r->leader_command_queue)) {
            case USER_OP_MALLOC:
                execute_malloc(n, l_r);
                break;
            case USER_OP_FREE:
                break;
            case USER_OP_WRITE:
                execute_write(n, l_r);
                break;
            case USER_OP_READ:
                execute_read(n, l_r);
                break;
            case USER_OP_DUMP:
                break;
            case USER_OP_SNAP:
                break;
            case USER_OP_DNET:
                break;
            case USER_OP_KILL:
                break;
            case USER_OP_REVIVE:
                break;
            case USER_OP_NONE:
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

        execute_command(n, l_r);

        if (die == 1)
            break;
    }
}
