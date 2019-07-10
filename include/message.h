#ifndef DISTRIBUTEDMALLOC_MESSAGE_H
#define DISTRIBUTEDMALLOC_MESSAGE_H

#include <stdlib.h>

enum operation {
    OP_OK = 0,
    OP_MALLOC,
    OP_FREE,
    OP_WRITE,
    OP_READ,
    OP_READ_FILE,
    OP_SNAP,
    OP_LEADER,
    OP_WHOISLEADER,
    OP_REVIVE,
    OP_KILL,
    OP_TEST,
    OP_NONE,
    OP_DUMP,
    OP_DUMP_ALL,
    OP_LEADER_OK,
    OP_ALIVE,
    OP_LEADER_AGAIN
};

struct message {
    unsigned short id_s; // Source message id
    unsigned short id_t; // Target message id
    unsigned short id_o; // Node id for the operation
    unsigned short need_callback; // 1 if the target node have to respond OP_ALIVE
    size_t address;
    size_t size;
    enum operation op;
};

struct message *generate_message(unsigned short id_s,
                                 unsigned short id_t,
                                 unsigned short id_o,
                                 size_t address,
                                 size_t size,
                                 enum operation op);

struct message *generate_message_a(unsigned short id_s,
                                   unsigned short id_t,
                                   unsigned short id_o,
                                   size_t address,
                                   size_t size,
                                   enum operation op,
                                   unsigned short need_callback);

#endif /* !DISTRIBUTEDMALLOC_MESSAGE_H */
