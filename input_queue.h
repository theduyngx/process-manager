//#include "pseudo_process.h"
#include "sort.h"

#define SIZE 15

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


/* heap data structure */
struct heap {
    int size;
    int max_size;
    process_t** arr;
};
typedef struct heap heap_t;

heap_t* heap_init();
void heap_insert(heap_t* h, process_t* p);
void build_max_heap(heap_t* h);
process_t* extract_max(heap_t* h);