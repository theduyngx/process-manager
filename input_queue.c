/*
 * Author  : The Duy Nguyen - 1100548
 * File    : input_queue.c
 * Purpose : Functions related to the input queue (the queue which contains all processes that
 *           request to be run).
 */

#include <stdlib.h>
#include <assert.h>
#include "input_queue.h"


qnode_t* queue_init() {
    qnode_t* q = (qnode_t*) malloc(sizeof(qnode_t));
    q->last = q;
    assert(q);
    return q;
}

void enqueue(qnode_t* q, process_t* p) {
    assert(q->last);
    q->last->process = p;
    q->last->next = queue_init();
    q->last = q->last->next;
}

qnode_t* dequeue(qnode_t** q) {
    qnode_t* popped = *q;
    (*q)->next->last = (*q)->last;
    (*q) = (*q)->next;
    return popped;
}
