#ifndef DISTRIBUTEDMALLOC_COMMAND_QUEUE_H
#define DISTRIBUTEDMALLOC_COMMAND_QUEUE_H

#include <stdlib.h>

#include "message.h"

// OP_MALLOC, // Data => Size
// OP_FREE, // Data => Address
// OP_WRITE, // Data => Address; Data size; Data
// OP_READ, // Data => Address; Data size
// OP_DUMP, // Data => Address
// OP_SNAP,
// OP_DNET,
// OP_KILL, // Data => id
// OP_REVIVE, // Data => id
// OP_NONE

struct command_queue {
    enum operation command;
    struct command_queue *next;
    void *data; // Contain the structure depending of the command
};


struct command_queue *generate_command_queue(enum operation op, void *data);

// Delete first command queue, return head
struct command_queue *pop_command(struct command_queue *head);

// Push command in queue, return head, can have head=NULL for init queue
struct command_queue *push_command(struct command_queue *head, struct command_queue *new_elt);

// Get command queue data, you should known how to cast it
void *peek_command(struct command_queue *head);

// Get user command enum of the first of the queue
enum operation peek_user_command(struct command_queue *head);

struct data_write *generate_data_write(size_t address, size_t size, void *data);

struct data_read *generate_data_read(size_t address, size_t size);

struct data_size *generate_data_size(size_t size);

struct data_id *generate_data_id(unsigned short id);

struct data_write {
    size_t address;
    size_t size;
    void *data;
};

struct data_read {
    size_t address;
    size_t size;
};

struct data_id {
    unsigned short id;
};

struct data_address {
    size_t address;
};

struct data_size {
    size_t size;
};

#endif /* !DISTRIBUTEDMALLOC_COMMAND_QUEUE_H */
