/*
 * Author  : The Duy Nguyen - 1100548
 * File    : ready_queue.h
 * Purpose : Header file for the ready queue abstraction. The ready queue is dynamic, and based on
 *           scheduling algorithm used, it will use a different data structure to manage processes.
 */

#ifndef PROJECT1_CLION_READY_QUEUE_H
#define PROJECT1_CLION_READY_QUEUE_H

#include "heap.h"
#include "queue.h"


/* Data structure */

// the possible data structures of the ready queue
enum structure {
    HEAP,
    QUEUE
};

// ready queue data type
struct ready_queue {
    enum structure type;
    int size;
    union {
        heap_t* h;
        queue_t* q;
    } list;
};
typedef struct ready_queue ready_queue_t;


/* Function prototypes */
ready_queue_t* ready_queue_init(enum structure type);
void insert(ready_queue_t* rq, process_t* p);
process_t* extract(ready_queue_t* rq);
void free_ready_queue(ready_queue_t* rq);

#endif //PROJECT1_CLION_READY_QUEUE_H
