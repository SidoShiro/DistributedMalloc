#ifndef DISTRIBUTEDMALLOC_NODE_H
#define DISTRIBUTEDMALLOC_NODE_H

#include <stdlib.h>

#define USER_NODE (0)

struct node {
    unsigned short id;
    unsigned char isleader; // 1 = is Leader, 0 = no
    // map; // From .dot file
    size_t size;
    /* LEADER ONLY */
    struct blocks *blk;
};

struct node *generate_node(unsigned short id, size_t size);

#endif /* !DISTRIBUTEDMALLOC_NODE_H */
