/*
 * Author  : The Duy Nguyen - 1100548
 * File    : heap.c
 * Purpose : Functions related to dynamically-sized heap of processes.
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "heap.h"


/**
 * Initialize the heap.
 * @return  the initialized heap
 */
heap_t* heap_init() {
    heap_t* h = (heap_t*) malloc(sizeof(heap_t));
    assert(h);
    h->arr = (process_t**) malloc(sizeof(process_t*) * SIZE);
    assert(h->arr);
    h->max_size = SIZE;
    h->size = 0;
    return h;
}


/**
 * Get the direct parent node's position of a given node index.
 * @param  i  given node index
 * @return    the direct parent node index
 */
int parent(int i) {
    if (i < 0) {
        fprintf(stderr, "ERROR - parent(): Index node is negative");
        exit(1);
    }
    return (i - (i > 0))/2;
}


/**
 * Min-heapify by recursively moving process at index i to its appropriate position.
 * @param  h  the heap
 * @param  i  index of node to have it position updated
 */
void min_heapify(heap_t* h, int i) {
    int n        = h->size;
    int left     = 2*i + 1;
    int right    = 2*i + 2;
    int curr_min = i;

    // check if the left node precedes the current node
    process_t** arr = h->arr;
    if (left <= n && process_precede(arr[left], arr[curr_min]))
        curr_min = left;

    // check if the right node proceeds the current node
    if (right <= n && process_precede(arr[right], arr[curr_min]))
        curr_min = right;

    // swap the smallest node with the current node and repeat this process until
    // fully heapified
    if (curr_min != i) {
        process_t* tmp = arr[i];
        arr[i] = arr[curr_min];
        arr[curr_min] = tmp;
        min_heapify(h, curr_min);
    }
}


/**
 * Insert a process into the heap.
 * @param h the heap
 * @param p the process
 */
void heap_push(heap_t* h, process_t* p) {
    // error handling
    if (h == NULL || p == NULL) {
        fprintf(stderr, "ERROR - heap_push: null input\n");
        exit(1);
    }

    int n = h->size;
    int exceeded = 0;

    while (n >= h->max_size) {
        exceeded = 1;
        h->max_size += SIZE;
    }
    if (exceeded) h->arr = (process_t**) realloc(h->arr, sizeof(process_t*) * h->max_size);

    // first insert the time at the last position of the array and move it up
    process_t** arr = h->arr;
    arr[n++] = p;

    // move up until the heap property satisfies
    int i = n-1;
    h->size = n;
    while (i > 0 && process_exceed(arr[parent(i)], arr[i])) {
        process_t* tmp = arr[parent(i)];
        arr[parent(i)] = arr[i];
        arr[i] = tmp;
        i = parent(i);
    }
}


/**
 * Extract the most prioritized process from heap
 * @param  h  the heap
 * @return    the extracted process
 */
process_t* heap_pop(heap_t* h) {
    if (h == NULL) {
        fprintf(stderr, "ERROR - heap_pop: null heap\n");
        exit(1);
    }
    if (h->size == 0) return NULL;
    process_t** arr = h->arr;
    process_t* prioritized = arr[0];

    // replace the first item with the last item
    arr[0] = arr[--(h->size)];

    // if the size reaches 0
    if (h->size == 0) {
        h->max_size = SIZE;
        h->arr = (process_t**) realloc(arr, sizeof(process_t*) * h->max_size);
    }
    else {
        int preceded = 0;
        // loop until we get appropriate max size
        while (h->size < h->max_size - 2*SIZE) {
            h->max_size -= SIZE;
            preceded = 1;
        }
        // reallocated memory as per max size
        if (preceded) h->arr = (process_t**) realloc(h->arr, sizeof(process_t*) * h->max_size);
        min_heapify(h, 0);
    }
    return prioritized;
}


/**
 * Free heap structure.
 * @param  h  the heap to have its memory freed
 */
void free_heap(heap_t* h) {
    free(h->arr);
    free(h);
}
