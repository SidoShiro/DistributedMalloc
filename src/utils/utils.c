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
           "     : v_address : parts : size\n"
           " --- : --------- : ----- : ----\n");
    size_t size = 0;
    size_t total_size = 0;
    for (size_t i = 0; i < a_r->count_alloc; i++) {
        size = 0;
        for (size_t j = 0; j < a_r->allocs[i]->number_parts; j++) {
            size += a_r->allocs[i]->parts[j]->size;
            total_size += size;
        }
        printf("%4zu : %9zu : %5zu : %zu\n"
               , i, a_r->allocs[i]->v_address_start, a_r->allocs[i]->number_parts, size);
    }
    printf(" ===   =========   =====   ====\n");
    printf(" Total size : %zu\n", total_size);
    printf(" ==============================\n");
    fflush(stdout);
}
