#ifndef DISTRIBUTEDMALLOC_GRAPH_H
#define DISTRIBUTEDMALLOC_GRAPH_H

#include <stdlib.h>

struct graph {
    int x;
    int y;
};


struct node {
    unsigned id;
    unsigned capacity;
    struct node* next;
};


struct adjency_matrix {
    unsigned **matrix;
    unsigned dimension;
};

struct adjency_matrix* dot_reader(char* path);

void free_adjency_matrix(struct adjency_matrix* a);

#endif //DISTRIBUTEDMALLOC_GRAPH_H
