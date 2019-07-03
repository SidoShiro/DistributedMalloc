#include "pending_messages.h"


struct pending_messages* pending_messages_init(struct message* m, struct pending_messages* next) {
    struct pending_messages *pm = calloc(1, sizeof(*pm));
    pm->m = m;
    pm->next = next;
    return pm;
}

void pending_messages_add(struct pending_messages* pm, struct message* m) { // push_back
    if (!pm) {
        pm->next = pending_messages_init(m, NULL);
        return;
    }
    while (pm->next)
        pm = pm->next;
    pm->next = pending_messages_init(m, NULL);
}

void pending_messages_free(struct pending_messages* pm) {
    free(pm->m);
    free(pm);
}

