#include "leader.h"

int free_memory(size_t address, struct leader_resources *l_r) {
    if (!l_r->leader_reg)
        return 0;
    struct allocation_register *reg = l_r->leader_reg;
    for (size_t i = 0; i < reg->count_alloc; i++) {
        for (size_t j = 0; j < reg->allocs[i]->number_parts; j++) {
            if (reg->allocs[i]->parts[j]->virtual_address <= address
                && reg->allocs[i]->parts[j]->virtual_address + reg->allocs[i]->parts[j]->size > address) {
                l_r->availaible_memory += size_of_allocation(reg->allocs[i]);
                // Set all block of this alloc as free
                for (j = 0; j < reg->allocs[i]->number_parts; j++) {
                    reg->allocs[i]->parts[j]->free = 0;
                }
                free(reg->allocs[i]->parts);
                free(reg->allocs[i]);
                reg->allocs[i] = NULL;
                return 0;
            }
        }
    }
    return 0;
}

size_t single_part(size_t size, struct leader_resources *l_r) {
    if (size >= l_r->max_memory || size >= l_r->availaible_memory)
        return SIZE_MAX;
    struct block_register *blks = l_r->leader_blks;
    // Merge free blocks (reform big blocks after frees)
    merge_free_block(blks);
    if (!blks && blks->nb_blocks > 0) {
        debug("ERROR not malloc blockS !!!", 0); //l_r->id);
        return SIZE_MAX;
    }
    // Single Part
    for (size_t i = 0; i < blks->nb_blocks; i++) {
        struct block *b = blks->blks[i];
        while (b != NULL) {
            if (0 == b->free && b->id != l_r->id && !is_in_dead_list(l_r, b->id)) {
                if (size == b->size) {
                    b->free = 1;
                    struct allocation *a = malloc(60 + sizeof(struct allocation));
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
                        struct allocation *a = malloc(60 + sizeof(struct allocation));
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
    return SIZE_MAX;
}

size_t multiple_part(size_t size, struct leader_resources *l_r)  {
    if (size >= l_r->max_memory || size >= l_r->availaible_memory)
        return SIZE_MAX;
    struct block_register *blks = l_r->leader_blks;
    // Merge free blocks (reform big blocks after frees)
    merge_free_block(blks);
    if (!blks && blks->nb_blocks > 0) {
        debug("ERROR not malloc blockS !!!", 0); //l_r->id);
        return SIZE_MAX;
    }
    // Multi - Parts
    struct allocation *a = malloc(60 + sizeof(struct allocation));
    a->number_parts = 0;
    a->parts = NULL;
    a->v_address_start = SIZE_MAX;
    a->parts = malloc((200) * sizeof(struct allocation *));
    ssize_t m_size = size;
    size_t m_t_size = 0;
    for (size_t i = 0; i < blks->nb_blocks; i++) {
        if (blks->blks && blks->blks[i]) {
            struct block *b = blks->blks[i];
            while (b && b->id != l_r->id && m_size > 0) {
                if (b && b->free == 0) {
                    b->free = 1;
                    if ((ssize_t) b->size >= m_size) {
                        m_t_size = m_size;
                        m_size = 0;
                        b = split_block_u(b, m_t_size);
                    } else {
                        m_size -= b->size;
                    }
                    if (a->v_address_start == SIZE_MAX)
                        a->v_address_start = b->virtual_address;
                    a->number_parts++;
                    a->parts[a->number_parts - 1] = b;
                }
                b = b->next;
            }
        }
        if (m_size <= 0) {
            add_allocation(l_r->leader_reg, a);
            l_r->availaible_memory -= size_of_allocation(a);
            return a->v_address_start;
        }
    }
    return SIZE_MAX;
}

/**
 * Allocated memory for the User, using free space of nodes
 * | X1 |     |  X2 | X1 |
 * @param size
 * @param l_r
 * @return 1 if okay, 0 if no blocks possible for this size
 */
size_t alloc_memory(size_t size, struct leader_resources *l_r) {
    size_t t = single_part(size, l_r);
    if (t == SIZE_MAX) {
        return multiple_part(size, l_r);
    } else {
        return t;
    }
}


void get_command(struct leader_resources *l_r, unsigned short user) {
    // int MPI_Irecv(void *buf, int count, MPI_Datatype datatype, int source,
    //              int tag, MPI_Comm comm, MPI_Request *request);
    struct message buff;
    int count = sizeof(struct message);
    MPI_Request r;
    MPI_Status st;
    MPI_Irecv(&buff, count, MPI_BYTE, MPI_ANY_SOURCE, TAG_MSG, MPI_COMM_WORLD, &r);

    if (0 == MPI_Wait(&r, &st)) {
        struct message *m = &buff;
        struct command_queue *n_command = generate_command_queue(m->op, NULL);
        switch (m->op) {
            case OP_IS_ALIVE:
                debug("Leader recv OP IS ALIVE from User", l_r->id);
                n_command->command = m->op;
                n_command->data = NULL;
                l_r->leader_command_queue = push_command(l_r->leader_command_queue, n_command);
                break;
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
                MPI_Irecv(wbuff, m->size * sizeof(char), MPI_BYTE, user, TAG_DATA, MPI_COMM_WORLD, &r);
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
            case OP_TABLE:
                debug("Leader recv OP TABLE", l_r->id);
                print_allocations_table(l_r);
                fflush(0);
                break;
            case OP_MALLOC:
                debug("Leader recv OP MALLOC from User", l_r->id);
                n_command->command = m->op;
                n_command->data = NULL;
                struct data_size *d_s = generate_data_size(m->size);
                n_command->data = d_s;
                l_r->leader_command_queue = push_command(l_r->leader_command_queue, n_command);
                break;
            case OP_FREE:
                debug("Leader recv OP FREE from User", l_r->id);
                n_command->command = m->op;
                n_command->data = NULL;
                struct data_address *d_a_free = generate_data_address(m->address);
                n_command->data = d_a_free;
                l_r->leader_command_queue = push_command(l_r->leader_command_queue, n_command);
                break;
            case OP_DUMP:
                debug("Leader recv OP DUMP from User", l_r->id);
                n_command->command = m->op;
                n_command->data = NULL;
                struct data_address *d_a_dump = generate_data_address(m->address);
                n_command->data = d_a_dump;
                l_r->leader_command_queue = push_command(l_r->leader_command_queue, n_command);
                break;
            case OP_DUMP_ALL:
                debug("Leader recv OP DUMP ALL from User", l_r->id);
                n_command->command = m->op;
                n_command->data = NULL;
                l_r->leader_command_queue = push_command(l_r->leader_command_queue, n_command);
                break;
            case OP_KILL: {
                debug("Leader recv OP KILL from User", l_r->id);
                n_command->command = m->op;
                n_command->data = generate_data_id(m->id_o);
                l_r->leader_command_queue = push_command(l_r->leader_command_queue, n_command);
                break;
            }
            case OP_REVIVE: {
                debug("Leader recv OP KILL from User", l_r->id);
                n_command->command = m->op;
                n_command->data = generate_data_id(m->id_o);
                l_r->leader_command_queue = push_command(l_r->leader_command_queue, n_command);
                break;
            }
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
            MPI_Isend((void *) m, sizeof(struct message), MPI_BYTE, m->id_t, TAG_MSG, MPI_COMM_WORLD, &r);
            free(m);
            return;
        }
        debug("FATAL ERROR", l_r->id);
        if (v_addr == 1999) {
            debug("SEC", l_r->id);
        }
        struct message *m = generate_message(l_r->id, DEF_NODE_USER, DEF_NODE_USER, SIZE_MAX, SIZE_MAX, OP_MALLOC);
        MPI_Request r;
        MPI_Isend((void *) m, sizeof(struct message), MPI_BYTE, m->id_t, TAG_MSG, MPI_COMM_WORLD, &r);
        free(m);
        return;
    }
    debug("Allocation at ", l_r->id);
    printf("%zu of %zu bytes\n", v_addr, sss);
    struct message *m = generate_message(l_r->id, DEF_NODE_USER, DEF_NODE_USER, v_addr, sss, OP_MALLOC);
    MPI_Request r;
    MPI_Isend((void *) m, sizeof(struct message), MPI_BYTE, m->id_t, TAG_MSG, MPI_COMM_WORLD, &r);
    free(m);
}

void execute_free(struct leader_resources *l_r) {
    struct data_address *d_a = peek_command(l_r->leader_command_queue);
    if (!d_a) {
        debug("ERROR allocation data_size for OP MALLOC execution [LEADER]", l_r->id);
        return;
    }
    int yes = free_memory(d_a->address, l_r);
    if (yes)
        debug("Free operation successful", l_r->id);
    else
        debug("Free operation was invalid", l_r->id);
}

void execute_read(struct leader_resources *l_r) {
    struct data_read *d_r;
    d_r = peek_command(l_r->leader_command_queue);

    size_t part_s = 0;
    struct allocation *c_a = give_for_v_address(l_r, d_r->address, &part_s);
    if (c_a == NULL) {
        debug("Seg Fault: requested read to a not allocated address", l_r->id);
        // Response negative to user
        struct message *mU = generate_message(l_r->id, DEF_NODE_USER, DEF_NODE_USER, 0, 0, OP_OK);
        MPI_Send(mU, sizeof(struct message), MPI_BYTE, mU->id_t, TAG_MSG, MPI_COMM_WORLD);
        return;
    }

    // Buffer of all READS bytes
    char *read_buff = malloc(sizeof(char) * (d_r->size + 1));

    // 2 Get the block to write to (Warning to multiple parts allocation)
    //                             (Warning to size bigger than block)
    size_t to_write_address_v = d_r->address;
    size_t nb_read = 0;
    size_t nb_read_size = 0;
    size_t x = d_r->size;
    size_t offset = 0;
    for (size_t i = part_s; x > 0 && i < c_a->number_parts; i++) {
        struct block *b = c_a->parts[i];
        if (!is_in_dead_list(l_r, b->id)) {
            // compute local address to write
            size_t local_address = 0;
            if (b->virtual_address == to_write_address_v) {
                local_address = b->node_address;
            } else {
                local_address += to_write_address_v - b->virtual_address + b->node_address;
            }

            // compute size to write for this block
            size_t to_read_size = 0;
            size_t b_size_with_offset = b->size - (local_address - b->node_address);
            if (x <= b_size_with_offset) {
                to_read_size = x;
                x = 0;
            } else {
                x -= b_size_with_offset;
                to_read_size = b_size_with_offset;
            }

            to_write_address_v += to_read_size;
            nb_read_size += to_read_size;
            // 3 Send READ OP to each node (Warning to the local address of the node, not the virtual)
            struct message *m = generate_message(l_r->id, b->id, b->id, local_address, to_read_size, OP_READ);
            debug("Send OP Read", l_r->id);
            MPI_Send(m, sizeof(struct message), MPI_BYTE, b->id, TAG_MSG, MPI_COMM_WORLD);
            void *buff = malloc(sizeof(char) * (to_read_size + 1));
            MPI_Status st;
            MPI_Recv(buff, to_read_size, MPI_BYTE, b->id, TAG_DATA, MPI_COMM_WORLD, &st);
            memcpy((void *) (read_buff + (offset * sizeof(char))), buff, to_read_size);
            offset += to_read_size;
            nb_read++;
            free(m);
        }
    }
    debug("READ AND ASSEMBLED", l_r->id);
    //  debug_n(read_buff, l_r->id, nb_read_size);
    // Send Result Read to User
    debug("Send to User OK + Data Read size", l_r->id);
    struct message *mU = generate_message(l_r->id, DEF_NODE_USER, DEF_NODE_USER, 0, nb_read_size, OP_OK);
    MPI_Send(mU, sizeof(struct message), MPI_BYTE, mU->id_t, TAG_MSG, MPI_COMM_WORLD);
    debug("Send to User Assemebled Read Data", l_r->id);
    MPI_Send(read_buff, nb_read_size, MPI_BYTE, mU->id_t, TAG_DATA, MPI_COMM_WORLD);
    free(mU);
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
    size_t offset = 0;
    for (size_t i = part_s; x > 0 && i < c_a->number_parts; i++) {
        struct block *b = c_a->parts[i];
        if (!is_in_dead_list(l_r, b->id)) {
            // compute local address to write
            size_t local_address = 0;
            if (b->virtual_address == to_write_address_v) {
                local_address = b->node_address;
            } else {
                local_address += to_write_address_v - b->virtual_address + b->node_address;
            }

            // compute size to write for this block
            size_t to_write_size = 0;
            size_t b_size_with_offset = b->size - (local_address - b->node_address);
            if (x <= b_size_with_offset) {
                to_write_size = x;
                x = 0;
            } else {
                x -= b_size_with_offset;
                to_write_size = b_size_with_offset;
            }

            to_write_address_v += to_write_size;

            // struct queue *q = queue_init();
            // printf("Size to send for Write %zu\n\n", to_write_size);
            // 3 Send Write OP to each node (Warning to the local address of the node, not the virtual)
            struct message *m = generate_message(l_r->id, b->id, b->id, local_address, to_write_size, OP_WRITE);
            debug("Send Write OP", l_r->id);
            MPI_Send(m, sizeof(struct message), MPI_BYTE, b->id, TAG_MSG, MPI_COMM_WORLD);
            struct message m2;
            MPI_Status st;
            MPI_Recv(&m2, sizeof(struct message), MPI_BYTE, b->id, TAG_MSG, MPI_COMM_WORLD, &st);
            debug("Send Data", l_r->id);
            // debug_n(d_w->data, l_r->id, d_w->size);
            MPI_Send((void *) ((char *) d_w->data + offset), to_write_size, MPI_BYTE, b->id, TAG_DATA, MPI_COMM_WORLD);
            offset += to_write_size;
        }
    }

    if (x > 0) {
        debug("Write asked is overflowing the allocated block", l_r->id);
    }
}

void execute_dump(struct leader_resources *l_r) {
    struct data_address *d_a;
    d_a = peek_command(l_r->leader_command_queue);
    // 1 Get correct allocation (Handle notFound)
    size_t part_s = 0;
    struct allocation *c_a = give_for_v_address(l_r, d_a->address, &part_s);
    if (c_a == NULL) {
        debug("Seg Fault: requested dump to a not allocated address", l_r->id);
        return;
    }
    size_t offset = 0;
    size_t a_size = size_of_allocation(c_a);
    char *dump = malloc(sizeof(char) * (a_size + 2));
    for (size_t i = 0; i < c_a->number_parts; i++) {
        struct block *b = c_a->parts[i];
        struct message *m = generate_message(l_r->id, b->id, b->id, b->node_address, b->size, OP_READ);
        debug("Send Read OP", l_r->id);
        MPI_Send(m, sizeof(struct message), MPI_BYTE, b->id, TAG_MSG, MPI_COMM_WORLD);
        void *buff = malloc(sizeof(char) * (b->size + 1));
        MPI_Status st;
        MPI_Recv(buff, b->size, MPI_BYTE, b->id, TAG_DATA, MPI_COMM_WORLD, &st);
        memcpy((void *) (dump + (offset * sizeof(char))), buff, b->size);
        offset += b->size;
    }

    // Dump done
    debug("Dump :", l_r->id);
    debug_n(dump, l_r->id, a_size + 1);
}

void execute_dump_all(struct leader_resources *l_r) {
    struct allocation_register *a_r = l_r->leader_reg;
    for (size_t i = 0; i < a_r->count_alloc; i++) {
        struct allocation *c_a = a_r->allocs[i];
        if (c_a) {
            size_t offset = 0;
            size_t a_size = size_of_allocation(c_a);
            char *dump = malloc(sizeof(char) * (a_size + 2));
            for (size_t i_ = 0; i_ < c_a->number_parts; i_++) {
                struct block *b = c_a->parts[i];
                struct message *m = generate_message(l_r->id, b->id, b->id, b->node_address, b->size, OP_READ);
                debug("Send Read OP", l_r->id);
                MPI_Send(m, sizeof(struct message), MPI_BYTE, b->id, TAG_MSG, MPI_COMM_WORLD);
                void *buff = malloc(sizeof(char) * (b->size + 1));
                MPI_Status st;
                MPI_Recv(buff, b->size, MPI_BYTE, b->id, TAG_DATA, MPI_COMM_WORLD, &st);
                memcpy((void *) (dump + (offset * sizeof(char))), buff, b->size);
                offset += b->size;
            }

            // Dump done
            debug("Dump :", l_r->id);
            debug_n(dump, l_r->id, a_size + 1);
        }
    }
}

void execute_command(struct leader_resources *l_r, int *die) {
    while (l_r->leader_command_queue && peek_user_command(l_r->leader_command_queue) != OP_NONE) {

        switch (peek_user_command(l_r->leader_command_queue)) {
            case OP_IS_ALIVE: {
                debug("EXECUTE OP ALIVE", l_r->id);
                struct message *m_alive = generate_message(l_r->id, 0, 0, 0, 0, OP_ALIVE);
                MPI_Send(m_alive, sizeof(*m_alive), MPI_BYTE, 0, TAG_MSG, MPI_COMM_WORLD);
                free(m_alive);
                break;
            }
            case OP_MALLOC:
                debug("EXECUTE OP MALLOC LEADER", l_r->id);
                execute_malloc(l_r);
                break;
            case OP_FREE:
                debug("EXECUTE OP FREE LEADER", l_r->id);
                execute_free(l_r);
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
                debug("EXECUTE OP DUMP, LEADER", l_r->id);
                execute_dump(l_r);
                break;
            case OP_DUMP_ALL:
                debug("EXECUTE OP DUMP ALL, LEADER", l_r->id);
                execute_dump_all(l_r);
                break;
            case OP_SNAP:
                break;
            case OP_KILL: {
                struct data_id *id = peek_command(l_r->leader_command_queue);
                printf("KILL %u\n", id->id);
                fflush(0);
                add_dead(l_r, id->id);
                if (id->id == l_r->id) {
                    debug("KILL LEADER __", id->id);
                    *die = 1;
                }
                else {
                    struct message *m_kill = generate_message(l_r->id, id->id, 0, 0, 0, OP_KILL);
                    MPI_Send(m_kill, sizeof(struct message), MPI_BYTE, id->id, TAG_MSG, MPI_COMM_WORLD);
                }
                break;
            }
            case OP_REVIVE: {
                struct data_id *id = peek_command(l_r->leader_command_queue);
                rm_dead(l_r, id->id);
                struct message *m_revive = generate_message(l_r->id, id->id, 0, 0, 0, OP_REVIVE);
                MPI_Send(m_revive, sizeof(struct message), MPI_BYTE, id->id, TAG_REVIVE, MPI_COMM_WORLD);
                break;
            }
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

    int die = 0;
    while (1) {
        // Get command from user
        get_command(l_r, terminal_id);
        // Execute Commands
        execute_command(l_r, &die);
        // Break on death
        if (die) {
            n->is_dead = 1;
            return;
        }
    }
}
