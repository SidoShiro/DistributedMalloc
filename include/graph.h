#ifndef DISTRIBUTEDMALLOC_GRAPH_H
#define DISTRIBUTEDMALLOC_GRAPH_H

#include <stdlib.h>


struct adjacency_matrix {
    unsigned **matrix;
    unsigned dimension;
};

struct linked_list {
    unsigned id;
    struct linked_list* next;
};

struct personal_map {
    struct linked_list** paths;
    unsigned nb_paths; // redundent but why not
    unsigned id; // redundent but why not
};



struct adjacency_matrix* dot_reader(char* path);
struct personal_map* compute_personal_map(struct adjacency_matrix* a, unsigned id);

void free_adjacency_matrix(struct adjacency_matrix* a);
void free_linked_list(struct linked_list* lkl); // really needs to be public ?
void free_personal_map(struct personal_map* pm);

#endif //DISTRIBUTEDMALLOC_GRAPH_H
