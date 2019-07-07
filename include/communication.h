#ifndef DISTRIBUTEDMALLOC_COMMUNICATION_H
#define DISTRIBUTEDMALLOC_COMMUNICATION_H

#include "message.h"

#include "queue.h"

// Number of attempts failed before considering the node as dead.
#define NB_ITER 60

/*
 * Send the given message 'm' to the destination node
 * Return 1 if it's a success, 0 if the destination node didn't respond in time
 */
int send_safe_message(struct message *m_send, struct queue *queue);

/*
 * Receive a message sent with 'send_safe_message'
 * then send a OP_OK to the source node
 */
void receive_message(struct message *m_recv);

#endif /* !DISTRIBUTEDMALLOC_COMMUNICATION_H */
