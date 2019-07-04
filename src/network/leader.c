#include "node.h"
#include "leader.h"
#include "globals.h"
#include "message.h"
#include "debug.h"

#include <mpi.h>

struct leader_resources *generate_leader_resources(size_t nb_nodes /*, struct node *n*/) {
    struct leader_resources *l_r = malloc(sizeof(struct leader_resources));
    l_r->leader_blks = init_nodes_same_size(nb_nodes, 8);
    l_r->leader_command_queue = NULL;
    l_r->leader_reg = generate_allocs(4);
    //l_r->id = n->id;
    return l_r;
}

int check_block_for_alloc(size_t  *res_addr, struct block *b, size_t size, struct leader_resources *l_r) {
    if (0 == b->free) {
        if (size == b->size) {
            b->free = 1;
            struct allocation *a = malloc(sizeof(struct allocation));
            a->number_parts = 1;
            a->v_address_start = b->virtual_address;
            a->parts = malloc(a->number_parts * sizeof(struct allocation*));
            a->parts[0] = b;
            add_allocation(l_r->leader_reg, a);
            *res_addr = b->virtual_address;
            return 1;
        }
        else if (size < b->size) {
            b->free = 1;
            b = split_block_u(b, size);
            struct allocation *a = malloc(sizeof(struct allocation));
            a->number_parts = 1;
            a->v_address_start = b->virtual_address;
            a->parts = malloc(a->number_parts * sizeof(struct allocation*));
            a->parts[0] = b;
            add_allocation(l_r->leader_reg, a);
            *res_addr = b->virtual_address;
            return 1;
        }
        else {

        }

    }
    return 0;
}

/**
 * Allocated memory for the User, using free space of nodes
 * | X1 |     |  X2 | X1 |
 * @param size
 * @param l_r
 * @return 1 if okay, 0 if no blocks possible for this size
 */
size_t alloc_memory(size_t size, struct leader_resources *l_r) {
    struct block_register *blks = l_r->leader_blks;
    if (!blks && blks->nb_blocks > 0) {
        debug("ERROR not malloc blockS !!!", 0); //l_r->id);
        return 999;
    }
    size_t address = 0;
    for (size_t i = 0; i < blks->nb_blocks; i++) {
        struct block *b = blks->blks[i];
        if (!b) {
            debug("ERROR not malloc block !!!", 0); // l_r->id);
            return 1999;
        }
        if (1 == check_block_for_alloc(&address, b, size, l_r))
            return address;
    }
    return 0;
}

struct address_search *search_at_address(size_t address, struct leader_resources *l_r) {
    struct block_register *bs = l_r->leader_blks;
    for (size_t i = 0; i < bs->nb_blocks; i++) {
        struct block *b = bs->blks[i];
        if (b->virtual_address >= address && b->virtual_address + b->size < address) {
            struct address_search *add_s = malloc(sizeof(struct address_search));
            add_s->size = b->size;
            add_s->v_address = b->virtual_address;
            add_s->n_address = b->node_address;
            add_s->r_address = address;
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
    MPI_Irecv(buff, count, MPI_BYTE, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &r);

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
                MPI_Irecv(wbuff, m->size * sizeof(char), MPI_BYTE, user, 0, MPI_COMM_WORLD, &r);
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
    struct data_size *d_s = peek_command(l_r->leader_command_queue);
    if (!d_s) {
        debug("ERROR allocation data_size for OP MALLOC execution [LEADER]", n->id);
    }
    size_t  v_addr = alloc_memory(d_s->size, l_r);
    if (v_addr == 999 || v_addr == 1999) {
        debug("FATAL ERROR", n->id);
        if (v_addr == 1999) {
            debug("SEC", n->id);
        }
        return;
    }
    debug("Allocation at ", n->id);
    printf("%zu\n", v_addr);
    struct message *m = generate_message(n->id, DEF_NODE_USER, DEF_NODE_USER, v_addr, d_s->size, OP_MALLOC);

    MPI_Request r;
    MPI_Isend((void *) m, sizeof(struct message), MPI_BYTE, m->id_t, 0, MPI_COMM_WORLD, &r);
    free(m);
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
                debug("EXECUTE OP MALLOC LEADER", n->id);
                execute_malloc(n, l_r);
                break;
            case OP_FREE:
                debug("EXECUTE OP FREE LEADER", n->id);
                break;
            case OP_WRITE:
                debug("EXECUTE OP WRITE LEADER", n->id);
                execute_write(n, l_r);
                break;
            case OP_READ:
                debug("EXECUTE OP READ LEADER", n->id);
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
        pop_command(l_r->leader_command_queue);
    }
}


void leader_loop(struct node *n, unsigned short terminal_id, unsigned short nb_nodes) {
    // Init leader resource
    struct leader_resources *l_r = generate_leader_resources(nb_nodes/*, n*/);

    for (size_t  i = 0; i < l_r->leader_blks->nb_blocks; i++) {
        struct block *b = l_r->leader_blks->blks[i];
        printf("%u\n", b->id);
    }

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
        // Get command from user
        get_command(n, l_r, terminal_id);
        debug("COMMANDS LISTEN DONE", n->id);
        // Execute Commands
        execute_command(n, l_r);
        debug("COMMANDS EXEC DONE", n->id);
        // Break on death
        if (die == 1)
            break;
    }
}
