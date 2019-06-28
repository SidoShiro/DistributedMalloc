#include "command_queue.h"

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
enum user_command peek_user_command(struct command_queue *head) {
    if (head)
        return head->command;
    return NONE;
}
