#include "node.h"

struct node *generate_node(unsigned short id, size_t size) {
    struct node *n = malloc(sizeof(struct node));
    n->id = id;
    n->size = size;
    n->isleader = 0;
    n->blk = NULL; // Reserved for leader
    return n;
}
