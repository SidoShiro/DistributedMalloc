#include "queue.h"

#include <stdlib.h>

struct queue *queue_init() {
    struct queue *q = calloc(1, sizeof(struct queue));
    q->first = calloc(1, sizeof(struct queue_node));
    return q;
}

void queue_push_back(struct queue *q, void *data) {
    struct queue_node *n = q->first;
    while (n->next)
        n = n->next;
    n->data = data;
    n->next = calloc(1, sizeof(struct queue_node));
}

void queue_free(struct queue *q) {
    struct queue_node *n = q->first;
    while (n) {
        struct queue_node *tmp = n->next;
        free(n);
        n = tmp;
    }
    free(q);
}

void *queue_pop(struct queue *q) {
    if (!q->first->next)
        return NULL;
    void *data = q->first->data;
    struct queue_node *tmp = q->first;
    q->first = q->first->next;
    free(tmp);
    return data;
}
