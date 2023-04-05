#ifndef PROJECT1_CLION_READY_QUEUE_H
#define PROJECT1_CLION_READY_QUEUE_H

#include "heap.h"
#include "queue.h"

enum structure {
    HEAP,
    QUEUE
};

struct ready_queue {
    enum structure type;
    int size;
    union {
        heap_t* h;
        queue_t* q;
    } list;
};
typedef struct ready_queue ready_queue_t;

ready_queue_t* ready_queue_init(enum structure type);
void insert(ready_queue_t* rq, process_t* p);
process_t* extract(ready_queue_t* rq);
void free_ready_queue(ready_queue_t* rq);

#endif //PROJECT1_CLION_READY_QUEUE_H
