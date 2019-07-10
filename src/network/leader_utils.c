#include "leader.h"


void add_dead(struct leader_resources *l_r, unsigned short id) {
    if (!l_r || !l_r->dead_list)
        return;
    struct dead_list *d_l = l_r->dead_list;
    for (size_t i = 0; i < d_l->max; i++) {
        if (d_l->dead_list[i] == 0) {
            d_l->dead_list[i] = id;
            return;
        }
    }
}

void rm_dead(struct leader_resources *l_r, unsigned short id) {
    if (!l_r || !l_r->dead_list)
        return;
    struct dead_list *d_l = l_r->dead_list;
    for (size_t i = 0; i < d_l->max; i++) {
        if (d_l->dead_list[i] == id) {
            d_l->dead_list[i] = 0;
        }
    }
}

int is_in_dead_list(struct leader_resources *l_r, unsigned short id) {
    if (!l_r || !l_r->dead_list)
        return 0;
    struct dead_list *d_l = l_r->dead_list;
    for (size_t i = 0; i < d_l->max; i++) {
        if (d_l->dead_list[i] == id) {
            return 1;
        }
    }
    return 0;
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
        if (reg->allocs[i]) {
            for (size_t j = 0; j < reg->allocs[i]->number_parts; j++) {
                if (reg->allocs[i]->parts[j]->virtual_address <= v_address
                    && reg->allocs[i]->parts[j]->virtual_address + reg->allocs[i]->parts[j]->size > v_address) {
                    *part = j;
                    return reg->allocs[i];
                }
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
    l_r->dead_list = malloc(sizeof(struct dead_list) + 34);
    l_r->dead_list->dead_list = calloc(nb_nodes + 10, sizeof(unsigned short));
    l_r->dead_list->max = nb_nodes;
    return l_r;
}


void print_allocations_table(struct leader_resources *l_r) {
    struct allocation_register *a_r = l_r->leader_reg;
    printf(" == ALLOCATIONS TABLE == \n"
           "     : v_address : parts : size     : node(s)\n"
           " --- : --------- : ----- : -------- : -------\n");
    size_t size = 0;
    size_t total_size = 0;
    for (size_t i = 0; i < a_r->count_alloc; i++) {
        if (a_r->allocs[i]) {
            size = size_of_allocation(a_r->allocs[i]);
            total_size += size;
            printf("%4zu : %9zu : %5zu : %8zu : ", i, a_r->allocs[i]->v_address_start, a_r->allocs[i]->number_parts,
                   size);
            for (size_t j = 0; j < a_r->allocs[i]->number_parts; j++) {
                if (j == a_r->allocs[i]->number_parts - 1)
                    printf("%d\n", a_r->allocs[i]->parts[j]->id);
                else
                    printf("%d > ", a_r->allocs[i]->parts[j]->id);
            }
        }
    }
    printf(" ===   =========   =====   ========   =======\n");
    printf(" Total size : %zu bytes\n", total_size);
    printf(" ============================================\n");
    fflush(stdout);
}

