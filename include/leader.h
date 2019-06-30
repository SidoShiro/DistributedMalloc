#ifndef DISTRIBUTEDMALLOC_LEADER_H
#define DISTRIBUTEDMALLOC_LEADER_H

#include <stdlib.h>

#include "node.h"
#include "message.h"
#include "command_queue.h"
#include "block.h"

struct leader_resources {
    struct blocks *leader_blks;
    struct command_queue *leader_command_queue;
};

#endif /* !DISTRIBUTEDMALLOC_LEADER_H */
