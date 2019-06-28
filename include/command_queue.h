#ifndef DISTRIBUTEDMALLOC_COMMAND_QUEUE_H
#define DISTRIBUTEDMALLOC_COMMAND_QUEUE_H

#include <stdlib.h>

enum user_command {
    MALLOC, // Data => Size
    FREE, // Data => Address
    WRITE, // Data => Address; Data size; Data
    READ, // Data => Address; Data size
    DUMP, // Data => Address
    SNAP,
    DNET,
    KILL, // Data => id
    REVIVE, // Data => id
    NONE
};

struct command_queue {
    enum user_command command;
    struct command_queue *next;
    void *data; // Contain the structure depending of the command
};

// Delete first command queue, return head
struct command_queue *pop_command(struct command_queue *head);

// Push command in queue, return head, can have head=NULL for init queue
struct command_queue *push_command(struct command_queue *head, struct command_queue *new_elt);

// Get command queue data, you should known how to cast it
void *peek_command(struct command_queue *head);

// Get user command enum of the first of the queue
enum user_command peek_user_command(struct command_queue *head);

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
