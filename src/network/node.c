#include "node.h"

struct node *generate_node(unsigned short id, size_t size) {
    struct node *n = malloc(sizeof(struct node));
    if (!n)
        return NULL;
    n->id = id;
    n->size = size;
    n->memory = malloc(size * sizeof(char));
    n->isleader = 0;
    return n;
}
