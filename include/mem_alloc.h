/*
 * Author  : The Duy Nguyen - 1100548
 * File    : mem_alloc.h
 * Purpose : Header for functions related to creating memory and allocating memory for processes.
 *           Included data structure of memory.
 */

#ifndef PROJECT1_MEM_ALLOC_H
#define PROJECT1_MEM_ALLOC_H

#include <stdint.h>
#include "pseudo_process.h"

#define SUCCESS 0
#define FAILURE 1
#define FINITE_CAPACITY 2048


/* data structures */

// whether memory is infinite or not
enum capacity_requirement {
    INF,
    FIN,
};

// possible states of a memory segment
enum segment_state {
    HOLE,
    PROCESS
};

// memory segment is stored as a doubly linked list
struct memseg {
    enum segment_state state;
    unsigned int size;
    unsigned int base;
    process_t* process;
    struct memseg *next;
    struct memseg *prev;
};
typedef struct memseg memseg_t;

// logical memory structure
struct memory {
    enum capacity_requirement requirement;
    unsigned int capacity;
    unsigned int used;
    int num_segments;
    memseg_t* segments;
};
typedef struct memory memory_t;


/* function prototypes */
memory_t* memory_inf_init();
memory_t* memory_init(unsigned int capacity);
int allocate_memory(memory_t* mem, process_t* p, unsigned int* base);
int deallocate_memory(memory_t* mem, process_t* p);
void free_memory(memory_t* mem);

#endif //PROJECT1_MEM_ALLOC_H
