#include "graph.h"
#include "utils.h"

struct linked_list* init_linked(unsigned id, struct linked_list* next);
void lkl_push_back(struct linked_list* lkl, unsigned id);
struct personal_map* init_personal_map(unsigned nb_paths, unsigned id);

unsigned* compute_distances(struct adjacency_matrix *a, unsigned id);
struct linked_list* add_childrens(struct adjacency_matrix* a, unsigned id, struct linked_list* lkl);

// Compute all shortests paths for the node 'id' to every others
struct personal_map* compute_personal_map(struct adjacency_matrix* a, unsigned id) {

    struct personal_map* pm = init_personal_map(a->dimension, id);
    
    unsigned* distances = compute_distances(a, id);
    distances = distances; // FIXME

    for (unsigned i = 1; i < pm->nb_paths; ++i) {
        // pm->paths[i] = compute_distances(a, id);
        // with the distance, comptute shortest paths
    }

    free(distances);

    return pm;
}


unsigned* compute_distances(struct adjacency_matrix *a, unsigned id) {

    unsigned* distances = calloc(a->dimension, sizeof(unsigned));
    distances[id] = 1; // just a security
    unsigned iteration = 1;

    // search childrens
    struct linked_list* childrens = add_childrens(a, id, NULL);
    struct linked_list* childrens_next_it = NULL;

    while (childrens) { // means : while (childrens_next_it)

        // for i in elm in the queue
        while (childrens) {

            if (distances[childrens->id] == 0) {
                // update distance of each one of them
                distances[childrens->id] = iteration;
                // add them to a queue // rofl, need to create one
                childrens_next_it = add_childrens(a, childrens->id, childrens_next_it);

            }
            // else : nothing

            struct linked_list* next = childrens->next;
            free(childrens);
            childrens = next;
        }
        iteration += 1;
        childrens = childrens_next_it;
        childrens_next_it = NULL;
    }

    return distances;
}

struct linked_list* add_childrens(struct adjacency_matrix* a, unsigned id, struct linked_list* lkl) {
    for (unsigned i = 0; i < a->dimension; ++i) {
        if (a->matrix[id][i]) {
            if (lkl) {
                lkl_push_back(lkl, i);
            }
            else {
                lkl = init_linked(i, NULL);
            }
        }
    }
    return lkl;
}




// UTILS: 
// TODO: in an other file



// linked_list: utils

struct linked_list* init_linked(unsigned id, struct linked_list* next) {
    struct linked_list* lkl = malloc(sizeof(*lkl));
    lkl->id = id;
    lkl->next = next;
    return lkl;
}

// use a queue if perfs is needed
void lkl_push_back(struct linked_list* lkl, unsigned id) {
    while (lkl->next) {
        lkl = lkl->next;
    }
    lkl->next = init_linked(id, NULL);
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
        pm->paths[i] = calloc(1, sizeof(struct linked_list));
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
