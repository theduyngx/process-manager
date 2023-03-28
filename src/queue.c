/*
 * Author  : The Duy Nguyen - 1100548
 * File    : queue.c
 * Purpose : Functions related to the queue structure.
 */

#include <stdlib.h>
#include <assert.h>
#include "queue.h"


/* initialize node */
qnode_t* qnode_init() {
    qnode_t* qn = (qnode_t*) malloc(sizeof(qnode_t));
    qn->last = qn;
    assert(qn);
    return qn;
}


/* initialize queue */
queue_t* queue_init() {
    queue_t* q = (queue_t*) malloc(sizeof(queue_t));
    q->node = qnode_init();
    q->size = 0;
    assert(q);
    return q;
}

/* enqueue */
void enqueue(queue_t* q, process_t* p) {
    assert(q);
    qnode_t* node = q->node;
    assert(node->last);
    p->p_status = READY;
    node->last->process = p;
    node->last->next = qnode_init();
    node->last = node->last->next;
    (q->size)++;
}

/* dequeue */
qnode_t* dequeue(queue_t* q) {
    if (q->size <= 0) {
        q->size = 0;
        return NULL;
    }
    qnode_t* popped = q->node;
    qnode_t** node = &(q->node);
    (*node)->next->last = (*node)->last;
    (*node) = (*node)->next;
    (q->size)--;
    return popped;
}
