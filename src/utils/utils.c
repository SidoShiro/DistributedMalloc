#include <debug.h>
#include "utils.h"
#include "leader.h"

int fun() {
    struct a as;
    as.x = 2;
    as.y = 1;
    return as.x;
}

size_t len(char **array) {
    size_t l = 0;
    char *x;
    if (array == NULL)
        return 0;
    x = array[l];
    while (x != NULL) {
        l++;
        x = array[l];
    }
    return l;
}
