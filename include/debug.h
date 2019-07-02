#ifndef DISTRIBUTEDMALLOC_DEBUG_H
#define DISTRIBUTEDMALLOC_DEBUG_H

#include <stdio.h>

void debug(char *msg, unsigned short id);

void debug_n(char *msg, unsigned short id, unsigned nb_bytes);

#endif /* !DISTRIBUTEDMALLOC_DEBUG_H */
