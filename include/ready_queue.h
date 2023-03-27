/*
 * Author  : The Duy Nguyen - 1100548
 * File    : ready_queue.h
 * Purpose : Header file for the ready queue, implemented using dynamically-sized heap.
 */

#ifndef PROJECT1_READY_QUEUE_H
#define PROJECT1_READY_QUEUE_H

#include "pseudo_process.h"

#define SIZE 15

/* heap data structure */
struct heap {
    int size;
    int max_size;
    process_t** arr;
};
typedef struct heap ready_t;

ready_t* ready_init();
void ready_insert(ready_t* h, process_t* p);
process_t* extract_max(ready_t* h);

#endif //PROJECT1_READY_QUEUE_H
