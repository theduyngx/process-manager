/*
 * Author  : The Duy Nguyen - 1100548
 * File    : queue.c
 * Purpose : Functions related to the queue structure.
 */

#include <stdlib.h>
#include <assert.h>
#include "queue.h"


/**
 * Initialize a queue's node.
 * @return  the initialized queue node
 */
qnode_t* qnode_init() {
    qnode_t* qn = (qnode_t*) malloc(sizeof(qnode_t));
    assert(qn);
    return qn;
}

/**
 * Initialize a queue.
 * @return  the initialized queue
 */
queue_t* queue_init() {
    queue_t* q = (queue_t*) malloc(sizeof(queue_t));
    q->node = qnode_init();
    q->last = q->node;
    q->size = 0;
    assert(q);
    return q;
}

/**
 * Enqueue a process to a given queue.
 * @param q  the given queue
 * @param p  the process to be enqueued
 */
void enqueue(queue_t* q, process_t* p) {
    assert(q);
    assert(q->last);
    if (! p) return;
    p->status = READY;
    q->last->process = p;
    q->last->next = qnode_init();
    q->last = q->last->next;
    (q->size)++;
}

/**
 * Dequeue a process from given queue.
 * @param q  the given queue
 * @param p  the dequeued process
 */
process_t* dequeue(queue_t* q) {
    if (q->size <= 0) {
        q->size = 0;
        return NULL;
    }
    qnode_t* popped = q->node;
    process_t* p = popped->process;
    q->node = popped->next;
    free(popped);
    (q->size)--;
    assert(q->size >= 0);
    return p;
}

/**
 * Free memory of given queue.
 * @param q  the queue
 */
void free_queue(queue_t* q) {
    while (q->size > 0) dequeue(q);
    free(q);
}
