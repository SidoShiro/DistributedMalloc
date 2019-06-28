#ifndef DISTRIBUTEDMALLOC_GRAPH_H
#define DISTRIBUTEDMALLOC_GRAPH_H

#include <stdlib.h>


struct adjacency_matrix {
    unsigned **matrix;
    unsigned dimension;
};

struct adjacency_matrix* dot_reader(char* path);

void free_adjacency_matrix(struct adjacency_matrix* a);

#endif //DISTRIBUTEDMALLOC_GRAPH_H
