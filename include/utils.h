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

#endif //DISTRIBUTEDMALLOC_UTILS_H
