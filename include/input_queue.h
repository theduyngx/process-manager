/*
 * Author  : The Duy Nguyen - 1100548
 * File    : input_queue.h
 * Purpose : Header file for input queue (the queue which contains all processes that request to
 *           be run). The input queue is implemented using the queue data structure.
 */

#ifndef PROJECT1_INPUT_QUEUE_H
#define PROJECT1_INPUT_QUEUE_H

#include "pseudo_process.h"

/* queue data structure */
struct qnode {
    process_t* process;
    struct qnode* next;
    struct qnode* last;
};
typedef struct qnode input_t;

input_t* input_init();
void enqueue(input_t* q, process_t* p);
input_t* dequeue(input_t** q);

#endif //PROJECT1_INPUT_QUEUE_H
