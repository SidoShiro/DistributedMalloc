#ifndef DISTRIBUTEDMALLOC_QUEUE_H
#define DISTRIBUTEDMALLOC_QUEUE_H

struct queue {
    struct queue_node *first;
};

struct queue_node {
    void *data;
    struct queue_node *next;
};

struct queue *queue_init();

void queue_push_back(struct queue *q, void *data);

void queue_free(struct queue *q);

void *queue_pop(struct queue *q);

#endif /* !DISTRIBUTEDMALLOC_QUEUE_H */
