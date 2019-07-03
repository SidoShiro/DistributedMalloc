#ifndef DISTRIBUTEDMALLOC_COMMUNICATION_H
#define DISTRIBUTEDMALLOC_COMMUNICATION_H

#include "message.h"

// Minimum waiting time before considering that the node is dead (ms)
#define TIMEOUT 5000

/*
 * Send the given message 'm' to the destination node
 * Return 1 if it's a success, 0 if the destination node didn't respond in time
 */
int send_safe_message(struct message *m);

/*
 *
 */
void receive_safe_message(struct message *m);

#endif /* !DISTRIBUTEDMALLOC_COMMUNICATION_H */
