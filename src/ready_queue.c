/*
 * Author  : The Duy Nguyen - 1100548
 * File    : ready_queue.c
 * Purpose : Functions related to ready queue abstraction. The ready queue is dynamic, and based on
 *           scheduling algorithm used, it will use a different data structure to manage processes.
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "ready_queue.h"

/**
 * Initialize the ready queue with specified structure.
 * @param type  structure type
 * @return      the initialized ready queue
 */
ready_queue_t* ready_queue_init(enum structure type) {
    ready_queue_t* rq = (ready_queue_t*) malloc(sizeof(ready_queue_t));
    rq->type = type;
    switch(type) {
        case HEAP:
            rq->list.h = heap_init();
            rq->size = rq->list.h->size;
            break;
        case QUEUE:
            rq->list.q = queue_init();
            rq->size = rq->list.q->size;
            break;
        default:
            fprintf(stderr, "Unknown type for ready queue\n");
            return NULL;
    }
    assert(rq);
    return rq;
}


/**
 * Insert a process to the ready queue.
 * @param rq  the ready queue
 * @param p   the process
 */
void insert(ready_queue_t* rq, process_t* p) {
    switch(rq->type) {
        case HEAP:
            heap_push(rq->list.h, p);
            rq->size = rq->list.h->size;
            break;
        case QUEUE:
            enqueue(rq->list.q, p);
            rq->size = rq->list.q->size;
            break;
    }
}


/**
 * Extract process with highest priority from the ready queue.
 * @param rq  the ready queue
 * @return    the extracted process
 */
process_t* extract(ready_queue_t* rq) {
    process_t* p;
    switch(rq->type) {
        case HEAP:
            p = heap_pop(rq->list.h);
            rq->size = rq->list.h->size;
            break;
        case QUEUE:
            p = dequeue(rq->list.q);
            rq->size = rq->list.q->size;
            break;
    }
    return p;
}


/**
 * Free memory of the ready queue.
 * @param rq  the ready queue
 */
void free_ready_queue(ready_queue_t* rq) {
    switch(rq->type) {
        case HEAP:
            free_heap(rq->list.h);
            break;
        case QUEUE:
            free_queue(rq->list.q);
            break;
    }
    free(rq);
}
