/*
 * Author  : The Duy Nguyen - 1100548
 * File    : ready_queue.c
 * Purpose : Functions related to handling the ready queue. The ready queue is implemented
 *           using dynamically-sized heap.
 */

#include <stdlib.h>
#include <assert.h>
#include "ready_queue.h"
#include "pseudo_process.h"

ready_t* ready_init() {
    ready_t* h = (ready_t*) malloc(sizeof(ready_t));
    assert(h);
    h->arr = (process_t**) malloc(sizeof(process_t*) * SIZE);
    h->max_size = SIZE;
    h->size = 0;
    return h;
}

// returns the index of the parent node
int parent(int i) {
    return (i-1)/2;
}


// insert the item at the appropriate position
void ready_insert(ready_t* h, process_t* p) {
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

// moves the item at position i of array a into its appropriate position
void min_heapify(ready_t* h, int i) {
    int n       = h->size;
    int left    = 2*i + 1;
    int right   = 2*i + 2;
    int largest = i;

    // check if the left node is larger than the current node
    process_t** arr = h->arr;
    if (left <= n && process_precede(arr[left], arr[largest]))
        largest = left;

    // check if the right node is larger than the current node
    if (right <= n && process_precede(arr[right], arr[largest]))
        largest = right;

    // swap the largest node with the current node and repeat this process until
    // the current node is larger than the right and the left node
    if (largest != i) {
        process_t* tmp = arr[i];
        arr[i] = arr[largest];
        arr[largest] = tmp;
        min_heapify(h, largest);
    }

}

// converts an array into a heap
void build_max_heap(ready_t* h) {
    int i;
    int n = h->size;
    for (i = n/2; i >= 0; i--)
        min_heapify(h, i);
}

// deletes the max item and return
process_t* extract_max(ready_t* h) {
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