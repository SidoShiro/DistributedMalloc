#include <debug.h>
#include "utils.h"
#include "leader.h"

int fun() {
    struct a as;
    as.x = 2;
    as.y = 1;
    return as.x;
}

size_t len(char **array) {
    size_t l = 0;
    char *x;
    if (array == NULL)
        return 0;
    x = array[l];
    while (x != NULL) {
        l++;
        x = array[l];
    }
    return l;
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
