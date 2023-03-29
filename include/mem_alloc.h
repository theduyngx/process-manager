
#ifndef PROJECT1_MEM_ALLOC_H
#define PROJECT1_MEM_ALLOC_H

#include <stdint.h>
#include "pseudo_process.h"

#define SUCCESS 0
#define FAILURE 1
#define MAX_CAPACITY 2048


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
    process_t* process;
    struct memseg *next;
    struct memseg *prev;
};
typedef struct memseg memseg_t;

// memory
struct memory {
    enum capacity_requirement requirement;
    unsigned int capacity;
    unsigned int used;
    int num_segments;
    memseg_t* segments;
};
typedef struct memory memory_t;


memory_t* memory_inf_init();
memory_t* memory_init(unsigned int capacity);
int allocate_memory(memory_t* mem, process_t* p);
int deallocate_memory(memory_t* mem, process_t* p);

#endif //PROJECT1_MEM_ALLOC_H
