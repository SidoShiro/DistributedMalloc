#include "message.h"

struct message *generate_message(unsigned short id_s,
                                 unsigned short id_t,
                                 unsigned short id_o,
                                 size_t address,
                                 size_t size,
                                 enum operation op) {
    struct message *m = malloc(sizeof(struct message));
    m->op = op;
    m->address = address;
    m->id_o = id_o;
    m->id_s = id_s;
    m->id_t = id_t;
    m->size = size;
    return m;
}
