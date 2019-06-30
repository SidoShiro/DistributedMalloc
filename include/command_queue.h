#ifndef DISTRIBUTEDMALLOC_COMMAND_QUEUE_H
#define DISTRIBUTEDMALLOC_COMMAND_QUEUE_H

#include <stdlib.h>

enum user_command {
    USER_OP_MALLOC, // Data => Size
    USER_OP_FREE, // Data => Address
    USER_OP_WRITE, // Data => Address; Data size; Data
    USER_OP_READ, // Data => Address; Data size
    USER_OP_DUMP, // Data => Address
    USER_OP_SNAP,
    USER_OP_DNET,
    USER_OP_KILL, // Data => id
    USER_OP_REVIVE, // Data => id
    USER_OP_NONE
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
