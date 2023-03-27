/*
 * Author  : The Duy Nguyen - 1100548
 * File    : input_queue.h
 * Purpose : Header file for input queue (the queue which contains all processes that request to
 *           be run). The input queue is implemented using the queue data structure.
 */


#include "pseudo_process.h"

/* queue data structure */
struct qnode {
    process_t* process;
    struct qnode* next;
    struct qnode* last;
};
typedef struct qnode qnode_t;

qnode_t* queue_init();
void enqueue(qnode_t* q, process_t* p);
qnode_t* dequeue(qnode_t** q);
