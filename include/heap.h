/*
 * Author  : The Duy Nguyen - 1100548
 * File    : heap_queue.h
 * Purpose : Header file for the ready queue, implemented using dynamically-sized heap.
 */

#ifndef PROJECT1_heap_QUEUE_H
#define PROJECT1_heap_QUEUE_H

#include "pseudo_process.h"

#define SIZE 15

/* heap data structure */
struct heap {
    int size;
    int max_size;
    process_t** arr;
};
typedef struct heap heap_t;

heap_t* heap_init();
void heap_push(heap_t* h, process_t* p);
process_t* heap_pop(heap_t* h);
void free_heap(heap_t* h);

#endif //PROJECT1_heap_QUEUE_H
