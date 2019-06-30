#include "graph.h"
#include "utils.h"

// private utils function
struct linked_list* init_linked(unsigned id, struct linked_list* next);
void lkl_push_back(struct linked_list* lkl, unsigned id);
struct personal_map* init_personal_map(unsigned nb_paths, unsigned id);
void show_what_you_got(struct linked_list* lkl);

// private core function 
unsigned* compute_distances(struct adjacency_matrix *a, unsigned id);
struct linked_list* add_childrens(struct adjacency_matrix* a, unsigned id, struct linked_list* lkl);
unsigned get_closest(unsigned* distances, struct linked_list* nodes);

// Compute all shortests paths for the node 'id' to every others
struct personal_map* compute_personal_map(struct adjacency_matrix* a, unsigned id) {

    struct personal_map* pm = init_personal_map(a->dimension, id);
    
    unsigned* distances = compute_distances(a, id);
    distances = distances; // FIXME

    for (unsigned i = 1; i < pm->nb_paths; ++i) { // Node 0 shouldn't exist. TODO: check that

        if (i == id)
            continue;

        // with the distance, comptute shortest paths;

        struct linked_list* path = NULL;
        unsigned current_node_id = i;

        // FIXME : Pas très opti, mais depmande bien 3~5h de plus pour tout opti
        while (current_node_id != id) {

            path = init_linked(current_node_id, path);

            // Get all 'parents' of this node (each node pointing to him)
            struct linked_list* parents = add_childrens(a, current_node_id, NULL);

            // for all of them, get the one with the lowest distance
            unsigned closest = get_closest(distances, parents);

            //path = init_linked(closest, path); // push front
            current_node_id = closest;
            free_linked_list(parents);
        }

        pm->paths[i] = path;

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

unsigned get_closest(unsigned* distances, struct linked_list* nodes) {
    unsigned min_id = nodes->id;
    unsigned min = distances[min_id];
    nodes = nodes->next; // fortement connexe so should work well

    while (nodes) {
        unsigned curr_dist = distances[nodes->id];
        if (curr_dist < min) {
            min = curr_dist;
            min_id = nodes->id;
        }
        nodes = nodes->next;
    }
    return min_id;
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

void show_what_you_got(struct linked_list* lkl) {
    while (lkl != NULL) {
        printf("%u ", lkl->id);
        lkl = lkl->next;
    }
    printf("\n");
}

// Personal map: utils

struct personal_map* init_personal_map(unsigned nb_paths, unsigned id) {
    struct personal_map* pm = malloc(sizeof(*pm));
    pm->paths = calloc(nb_paths, sizeof(struct linked_list*));
    pm->nb_paths = nb_paths;
    pm->id = id;
    return pm;
}

void free_personal_map(struct personal_map* pm) {
    for(unsigned i=0; i < pm->nb_paths; ++i) {
        if (pm->paths[i])
            free_linked_list(pm->paths[i]);
    }
    free(pm->paths);
    free(pm);
}
