/*
 * Author  : The Duy Nguyen - 1100548
 * File    : input_queue.c
 * Purpose : Functions related to the input queue (the queue which contains all processes that
 *           request to be run).
 */

#include <stdlib.h>
#include <assert.h>
#include "input_queue.h"


input_t* input_init() {
    input_t* q = (input_t*) malloc(sizeof(input_t));
    q->last = q;
    assert(q);
    return q;
}

void enqueue(input_t* q, process_t* p) {
    assert(q->last);
    p->p_status = READY;
    q->last->process = p;
    q->last->next = input_init();
    q->last = q->last->next;
}

input_t* dequeue(input_t** q) {
    input_t* popped = *q;
    (*q)->next->last = (*q)->last;
    (*q) = (*q)->next;
    return popped;
}
