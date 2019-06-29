#include "graph.h"

struct linked_list* init_linked(unsigned id, struct linked_list* next);
struct personal_map* init_personal_map(unsigned nb_paths, unsigned id);



// Compute all shortests paths for the node 'id' to every others
struct personal_map* compute_personal_map(struct adjacency_matrix* a, unsigned id) {

    struct personal_map* pm = init_personal_map(a->dimension, id);
    
    // TODO


    return pm;
}

// linked_list: utils

struct linked_list* init_linked(unsigned id, struct linked_list* next) {
    struct linked_list* lkl = malloc(sizeof(*lkl));
    lkl->id = id;
    lkl->next = next;
    return lkl;
}

void free_linked_list(struct linked_list* lkl) {
    while (lkl != NULL) {
        struct linked_list* lkl2 = lkl->next;
        free(lkl);
        lkl = lkl2;
    }
}

// Personal map: utils

struct personal_map* init_personal_map(unsigned nb_paths, unsigned id) {
    struct personal_map* pm = malloc(sizeof(*pm));
    pm->paths = malloc(sizeof(struct linked_list*) * nb_paths);
    for (unsigned i = 0; i < nb_paths; ++i) {
        pm->paths[i] = malloc(sizeof(struct linked_list));
    }
    pm->nb_paths = nb_paths;
    pm->id = id;
    return pm;
}

void free_personal_map(struct personal_map* pm) {
    for(unsigned i=0; i < pm->nb_paths; ++i) {
        free_linked_list(pm->paths[i]);
    }
    free(pm->paths);
    free(pm);
}
