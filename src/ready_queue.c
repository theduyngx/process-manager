
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "ready_queue.h"

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
