#ifndef DISTRIBUTEDMALLOC_UTILS_H
#define DISTRIBUTEDMALLOC_UTILS_H

#include <stdlib.h>
#include <stdio.h>
#include "leader.h"

struct a {
    int x;
    int y;
};

int fun();

size_t len(char **array);

void print_allocations_table(struct leader_resources *l_r);

#endif //DISTRIBUTEDMALLOC_UTILS_H
