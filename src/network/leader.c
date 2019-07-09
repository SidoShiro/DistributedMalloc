#include "node.h"
#include "leader.h"
#include "globals.h"
#include "message.h"
#include "debug.h"
#include "utils.h"

#include <sys/types.h>
#include <unistd.h>
#include <mpi.h>
#include <stdint.h>
#include <communication.h>

size_t get_message_size() {
    return (sizeof(unsigned short) * 3 + 2 * sizeof(size_t) + sizeof(enum operation));
}


size_t size_of_allocation(struct allocation *a) {
    size_t size = 0;
    if (!a)
        return size;
    for (size_t i = 0; i < a->number_parts; i++) {
        size += a->parts[i]->size;
    }
    return size;
}


struct allocation *give_for_v_address(struct leader_resources *l_r, size_t v_address, size_t *part) {
    if (!l_r->leader_reg)
        return NULL;
    struct allocation_register *reg = l_r->leader_reg;
    for (size_t i = 0; i < reg->count_alloc; i++) {
        for (size_t j = 0; j < reg->allocs[i]->number_parts; j++) {
            // FIXME check if its ok
            printf("%zu == %zu  \n\n", v_address, reg->allocs[i]->parts[j]->virtual_address);
            if (reg->allocs[i]->parts[j]->virtual_address <= v_address
                && reg->allocs[i]->parts[j]->virtual_address + reg->allocs[i]->parts[j]->size > v_address) {
                *part = j;
                return reg->allocs[i];
            }
        }
    }
    return NULL;
}

struct leader_resources *generate_leader_resources(size_t nb_nodes, size_t id) {
    struct leader_resources *l_r = malloc(128);
    l_r->leader_blks = init_nodes_same_size(nb_nodes, DEF_NODE_SIZE);
    l_r->leader_command_queue = NULL;
    l_r->leader_reg = generate_allocs(4);
    l_r->id = id;
    l_r->max_memory = nb_nodes * DEF_NODE_SIZE - (DEF_NODE_SIZE); // minus leader
    l_r->availaible_memory = l_r->max_memory;
    return l_r;
}

/**
 * Allocated memory for the User, using free space of nodes
 * | X1 |     |  X2 | X1 |
 * @param size
 * @param l_r
 * @return 1 if okay, 0 if no blocks possible for this size
 */
size_t alloc_memory(size_t size, struct leader_resources *l_r) {
    if (size >= l_r->max_memory || size >= l_r->availaible_memory)
        return SIZE_MAX;
    struct block_register *blks = l_r->leader_blks;
    if (!blks && blks->nb_blocks > 0) {
        debug("ERROR not malloc blockS !!!", 0); //l_r->id);
        return 999;
    }
    // Single Part
    for (size_t i = 0; i < blks->nb_blocks; i++) {
        struct block *b = blks->blks[i];
        while (b != NULL) {
            if (0 == b->free && b->id != l_r->id) {
                if (size == b->size) {
                    b->free = 1;
                    struct allocation *a = malloc(32 + sizeof(struct allocation));
                    a->number_parts = 1;
                    a->v_address_start = b->virtual_address;
                    a->parts = malloc(34 + (a->number_parts * sizeof(struct allocation *)));
                    a->parts[0] = b;
                    add_allocation(l_r->leader_reg, a);
                    l_r->availaible_memory -= size_of_allocation(a);
                    return b->virtual_address;
                } else if (size < b->size) {
                    b->free = 1;
                    b = split_block_u(b, size);
                    if (b) {
                        struct allocation *a = malloc(32 + sizeof(struct allocation));
                        a->number_parts = 1;
                        a->v_address_start = b->virtual_address;
                        a->parts = malloc(34 + (a->number_parts * sizeof(struct allocation *)));
                        a->parts[0] = b;
                        add_allocation(l_r->leader_reg, a);
                        l_r->availaible_memory -= size_of_allocation(a);
                        return b->virtual_address;
                    } else {
                        debug("Fatal Error, split invalid or memory error", 1);
                    }
                }
            }
            b = b->next;
        }
    }
    // Multi - Parts
    struct allocation *a = malloc(32 + sizeof(struct allocation));
    a->number_parts = 0;
    a->parts = NULL;
    a->v_address_start = SIZE_MAX;
    ssize_t m_size = size;
    for (size_t i = 0; i < blks->nb_blocks; i++) {
        struct block *b = blks->blks[i];
        while (b && b->id != l_r->id && m_size > 0) {
            if (b->free == 0) {
                b->free = 1;
                m_size -= b->size;
                if (m_size < 0) {
                    b = split_block_u(b, -1 * m_size);
                }
                if (a->v_address_start == SIZE_MAX)
                    a->v_address_start = b->virtual_address;
                a->number_parts++;
                a->parts = realloc(a->parts, 34 + (a->number_parts * sizeof(struct allocation *)));
                a->parts[a->number_parts - 1] = b;
            }
            b = b->next;
        }
        if (m_size <= 0) {
            add_allocation(l_r->leader_reg, a);
            l_r->availaible_memory -= size_of_allocation(a);
            return a->v_address_start;
        }
    }
    return SIZE_MAX;
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


void get_command(struct leader_resources *l_r, unsigned short user) {
    // int MPI_Irecv(void *buf, int count, MPI_Datatype datatype, int source,
    //              int tag, MPI_Comm comm, MPI_Request *request);
    struct message buff;
    int count = sizeof(struct message);
    MPI_Request r;
    MPI_Status st;
    MPI_Irecv(&buff, count, MPI_BYTE, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &r);

    if (0 == MPI_Wait(&r, &st)) {
        struct message *m = &buff;
        struct command_queue *n_command = generate_command_queue(m->op, NULL);
        switch (m->op) {
            case OP_OK:
                if (m->id_s == 0) {
                    debug("Leader recv OP OK from User", l_r->id);
                } else {
                    debug("Leader recv OP OK from a node", l_r->id);
                }
                break;
            case OP_WRITE:
                debug("Leader recv OP WRITE from User", l_r->id);
                n_command->command = m->op;
                n_command->data = NULL;
                struct data_write *d_w = generate_data_write(m->address, m->size, NULL);
                void *wbuff = malloc(sizeof(char) * (m->size + 2));
                debug("Leader wait DATA from User for OP WRITE", l_r->id);
                MPI_Irecv(wbuff, m->size * sizeof(char), MPI_BYTE, user, 0, MPI_COMM_WORLD, &r);
                if (0 == MPI_Wait(&r, &st)) {
                    d_w->data = wbuff;
                    n_command->data = d_w;
                }
                debug("Leader recv DATA from User for OP WRITE : ", l_r->id);
                debug_n(d_w->data, l_r->id, d_w->size);
                l_r->leader_command_queue = push_command(l_r->leader_command_queue, n_command);
                break;
            case OP_READ:
                debug("Leader recv OP READ from User", l_r->id);
                n_command->command = m->op;
                n_command->data = NULL;
                struct data_read *d_r = generate_data_read(m->address, m->size);
                n_command->data = d_r;
                l_r->leader_command_queue = push_command(l_r->leader_command_queue, n_command);
                break;
            case OP_MALLOC:
                debug("Leader recv OP MALLOC from User", l_r->id);
                n_command->command = m->op;
                n_command->data = NULL;
                struct data_size *d_s = generate_data_size(m->size);
                n_command->data = d_s;
                l_r->leader_command_queue = push_command(l_r->leader_command_queue, n_command);
                break;
            default:
                debug("Leader recv ANY OP from User", l_r->id);
                break;
        }
    }
    //free(buff);
}

void execute_malloc(struct leader_resources *l_r) {
    struct data_size *d_s = peek_command(l_r->leader_command_queue);
    if (!d_s) {
        debug("ERROR allocation data_size for OP MALLOC execution [LEADER]", l_r->id);
        return;
    }
    size_t v_addr = alloc_memory(d_s->size, l_r);
    size_t sss = d_s->size;
    if (v_addr == 999 || v_addr == 1999 || v_addr == SIZE_MAX) {
        if (v_addr == SIZE_MAX) {
            debug("Out of memory", l_r->id);
            struct message *m = generate_message(l_r->id, DEF_NODE_USER, DEF_NODE_USER, SIZE_MAX, 0, OP_MALLOC);
            MPI_Request r;
            MPI_Isend((void *) m, sizeof(struct message), MPI_BYTE, m->id_t, 0, MPI_COMM_WORLD, &r);
            free(m);
            return;
        }
        debug("FATAL ERROR", l_r->id);
        if (v_addr == 1999) {
            debug("SEC", l_r->id);
        }
        struct message *m = generate_message(l_r->id, DEF_NODE_USER, DEF_NODE_USER, SIZE_MAX, SIZE_MAX, OP_MALLOC);
        MPI_Request r;
        MPI_Isend((void *) m, sizeof(struct message), MPI_BYTE, m->id_t, 0, MPI_COMM_WORLD, &r);
        free(m);
        return;
    }
    debug("Allocation at ", l_r->id);
    printf("%zu of %zu bytes\n", v_addr, sss);
    struct message *m = generate_message(l_r->id, DEF_NODE_USER, DEF_NODE_USER, v_addr, sss, OP_MALLOC);
    MPI_Request r;
    MPI_Isend((void *) m, sizeof(struct message), MPI_BYTE, m->id_t, 0, MPI_COMM_WORLD, &r);
    free(m);
}

void execute_read(struct leader_resources *l_r) {
    struct data_read *d_r;
    d_r = peek_command(l_r->leader_command_queue);

    size_t part_s = 0;
    struct allocation *c_a = give_for_v_address(l_r, d_r->address, &part_s);
    if (c_a == NULL) {
        debug("Seg Fault: requested read to a not allocated address", l_r->id);
        return;
    }

    // Readed bytes
    char *read_buff = NULL;

    // 2 Get the block to write to (Warning to multiple parts allocation)
    //                             (Warning to size bigger than block)
    size_t to_write_address_v = d_r->address;
    size_t nb_read = 0;
    for (size_t i = part_s; i < c_a->number_parts; i++) {
        // TODO handle size overflow


        struct block *b = c_a->parts[i];
        // compute size to write for this block
        size_t to_read_size = d_r->size - b->size;
        d_r->size -= to_read_size;
        // compute local address to write
        size_t local_address = b->virtual_address - to_write_address_v;
        to_write_address_v += local_address;

        // 3 Send READ OP to each node (Warning to the local address of the node, not the virtual)
        struct message *m = generate_message(l_r->id, b->id, b->id, local_address, to_read_size, OP_READ);
        MPI_Send(m, sizeof(struct message), MPI_BYTE, b->id, 5, MPI_COMM_WORLD);
        nb_read++;
    }

    // 4 Receive bytes, append data from all nodes
    for (size_t i = 0; i < nb_read; i++) {
        // MPI_Irecv()
    }
    (void) read_buff;

}

void execute_write(struct leader_resources *l_r) {
    // MPI_Request r;
    // MPI_Status st;
    struct data_write *d_w;
    d_w = peek_command(l_r->leader_command_queue);
    // d_w.address is the virtual address on the network
    //    .size size in bytes of data
    //    .data

    // 1 Get correct allocation (Handle notFound)
    size_t part_s = 0;
    struct allocation *c_a = give_for_v_address(l_r, d_w->address, &part_s);
    if (c_a == NULL) {
        debug("Seg Fault: requested write to a not allocated address", l_r->id);
        return;
    }

    // 2 Get the block to write to (Warning to multiple parts allocation)
    //                             (Warning to size bigger than block)
    size_t to_write_address_v = d_w->address;
    size_t x = d_w->size;
    for (size_t i = part_s; x > 0 && i < c_a->number_parts; i++) {
        // TODO handle size overflow
        struct block *b = c_a->parts[i];
        // compute size to write for this block
        size_t to_write_size = 0;
        if (x <= b->size) {
            to_write_size = x;
            x = 0;
        } else {
            x -= b->size;
            to_write_size = b->size;
        }

        // compute local address to write
        size_t local_address = 0;
        if (b->virtual_address == to_write_address_v) {
            local_address = 0;
            to_write_address_v += to_write_size;
        } else {
            local_address += to_write_address_v - b->virtual_address;
            to_write_address_v += to_write_size;
        }

        // struct queue *q = queue_init();
        // printf("Size to send for Write %zu\n\n", to_write_size);
        // 3 Send Write OP to each node (Warning to the local address of the node, not the virtual)
        struct message *m = generate_message(l_r->id, b->id, b->id, local_address, to_write_size, OP_WRITE);
        debug("Send Write OP", l_r->id);
        MPI_Send(m, sizeof(struct message), MPI_BYTE, b->id, 3, MPI_COMM_WORLD);
        struct message m2;
        MPI_Status st;
        MPI_Recv(&m2, sizeof(struct message), MPI_BYTE, b->id, 3, MPI_COMM_WORLD, &st);
        debug("Send Data", l_r->id);
        // debug_n(d_w->data, l_r->id, d_w->size);
        MPI_Send(d_w->data, to_write_size, MPI_BYTE, b->id, 4, MPI_COMM_WORLD);
    }

    if (x > 0) {
        debug("Write asked is overflowing the allocated block", l_r->id);
    }

    // TODO Confirmation ?
    // struct message *m = generate_message(n->id, )
    // MPI_Isend()
}

void execute_command(struct leader_resources *l_r) {
    if (peek_user_command(l_r->leader_command_queue) != OP_NONE) {

        switch (peek_user_command(l_r->leader_command_queue)) {
            case OP_MALLOC:
                debug("EXECUTE OP MALLOC LEADER", l_r->id);
                execute_malloc(l_r);
                break;
            case OP_FREE:
                debug("EXECUTE OP FREE LEADER", l_r->id);
                break;
            case OP_WRITE:
                debug("EXECUTE OP WRITE LEADER", l_r->id);
                execute_write(l_r);
                break;
            case OP_READ:
                debug("EXECUTE OP READ LEADER", l_r->id);
                execute_read(l_r);
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
        l_r->leader_command_queue = pop_command(l_r->leader_command_queue);
    }
}


void leader_loop(struct node *n, unsigned short terminal_id, unsigned short nb_nodes) {
    // Init leader resource
    printf("NB NODES :%d,", nb_nodes);

    debug("START LEADER LOOP", n->id);
    struct leader_resources *l_r = generate_leader_resources(nb_nodes, n->id);

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
        print_allocations_table(l_r);
        // Get command from user
        get_command(l_r, terminal_id);
        // debug("COMMANDS LISTEN DONE", n->id);
        // Execute Commands
        execute_command(l_r);
        // debug("COMMANDS EXEC DONE", n->id);
        // Break on death
        if (die == 1)
            break;
    }
}
