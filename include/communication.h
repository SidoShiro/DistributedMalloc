#ifndef DISTRIBUTEDMALLOC_COMMUNICATION_H
#define DISTRIBUTEDMALLOC_COMMUNICATION_H

#include "message.h"

#include "queue.h"

// Number of attempts failed before considering the node as dead.
#define NB_ITER 60

#define TAG_MSG 3
#define TAG_DATA 4

/*
 * Send the given message 'm' to the destination node
 * Return 1 if it's a success, 0 if the destination node didn't respond in time
 */
int send_safe_message(struct message *m_send, struct queue *queue);

/*
 * Receive a message sent with 'send_safe_message'
 * then send a OP_OK to the source node
 */
struct message *receive_message(struct queue *message_queue);

/*
 * Recieve a specific data from a specific node.
 * All others messages recieved are put in the queue. If needed, a IS_ALIVE is send.
 * Return the wanted data, which was calloc.
 */
void *recieve_data(size_t size, struct queue *queue, unsigned source, void *data);

#endif /* !DISTRIBUTEDMALLOC_COMMUNICATION_H */
