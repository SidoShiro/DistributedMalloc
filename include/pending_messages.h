#ifndef DISTRIBUTEDMALLOC_PENDING_MESSAGES_H
#define DISTRIBUTEDMALLOC_PENDING_MESSAGES_H

#include "message.h"


struct pending_messages { // linked
    struct message *m;
    struct pending_message* next;
};

struct pending_messages* pending_messages_init(struct message* m, struct pending_messages* next);

void pending_messages_add(struct pending_messages* pm, struct message* m);

//struct message* pending_messages_pop(struct pending_messages* pm);

void pending_messages_free(struct pending_messages* pm);

#endif