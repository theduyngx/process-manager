/*
 * Author  : The Duy Nguyen - 1100548
 * File    : input_queue.h
 * Purpose : Header file for input queue (the queue which contains all processes that request to
 *           be run). The input queue is implemented using the queue data structure.
 */

#ifndef PROJECT1_INPUT_QUEUE_H
#define PROJECT1_INPUT_QUEUE_H

#include "pseudo_process.h"

/* node data structure */
struct qnode {
    process_t* process;
    struct qnode* next;
};
typedef struct qnode qnode_t;

/* queue data structure */
struct queue {
    qnode_t* node;
    qnode_t* last;
    int size;
};
typedef struct queue queue_t;

/* queue functions */
queue_t* queue_init();
void enqueue(queue_t* q, process_t* p);
process_t* dequeue(queue_t* q);
void free_queue(queue_t* q);

#endif //PROJECT1_INPUT_QUEUE_H
