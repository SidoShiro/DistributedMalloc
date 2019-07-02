#include "command_queue.h"

struct command_queue *generate_command_queue(enum operation op, void *data) {
    struct command_queue *q = malloc(sizeof(struct command_queue));
    q->data = data;
    q->command = op;
    q->next = NULL;
    return q;
}

// Delete first command queue, return head
struct command_queue *pop_command(struct command_queue *head) {
    struct command_queue *new_head = NULL;
    if (!head)
        return NULL;
    else if (!head->next) {
        if (head->data)
            free(head->data);
        free(head);
        return NULL;
    } else {
        new_head = head->next;
        if (head->data)
            free(head->data);
        free(head);
        return new_head;
    }
}

// Push command in queue, return head, can have head=NULL for init queue
struct command_queue *push_command(struct command_queue *head, struct command_queue *new_elt) {
    if (!head) {
        return new_elt;
    }
    struct command_queue *iter = head;
    while (iter->next) {
        iter = iter->next;
    }
    iter->next = new_elt;
    return head;
}

// Get command queue data, you should known how to cast it
void *peek_command(struct command_queue *head) {
    if (head)
        return head->data;
    return NULL;
}

// Get user command enum of the first of the queue
enum operation peek_user_command(struct command_queue *head) {
    if (head)
        return head->command;
    return OP_NONE;
}

struct data_write *generate_data_write(size_t address, size_t size, void *data) {
    struct data_write *d_w = malloc(sizeof(struct data_write));
    d_w->size = size;
    d_w->data = data;
    d_w->address = address;
    return d_w;
}


struct data_read *generate_data_read(size_t address, size_t size) {
    struct data_read *d_r = malloc(sizeof(struct data_read));
    d_r->size = size;
    d_r->address = address;
    return d_r;
}

struct data_size *generate_data_size(size_t size) {
    struct data_size *d_s = malloc(sizeof(struct data_size));
    d_s->size = size;
    return d_s;
}

struct data_id *generate_data_id(unsigned short id) {
    struct data_id *d_i = malloc(sizeof(struct data_id));
    d_i->id = id;
    return d_i;
}
