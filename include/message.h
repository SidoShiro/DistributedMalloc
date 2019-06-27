#ifndef DISTRIBUTEDMALLOC_MESSAGE_H
#define DISTRIBUTEDMALLOC_MESSAGE_H

#include <stdlib.h>

enum operation {
    OK = 0,
    WRITE,
    READ,
    SNAP,
    LEADER,
    WHOISLEADER,
    REVIVE,
    DIE,
    TEST
};

struct message {
    unsigned short id_s; // Source message id
    unsigned short id_t; // Target message id
    unsigned short id_o; // Node id for the operation
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

#endif /* !DISTRIBUTEDMALLOC_MESSAGE_H */
