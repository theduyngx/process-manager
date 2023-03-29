
#ifndef PROJECT1_MEM_ALLOC_H
#define PROJECT1_MEM_ALLOC_H

#include <stdint.h>
#include "pseudo_process.h"

#define SUCCESS 0
#define FAILURE 1


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
    uint32_t size;
    process_t** p_address;
    struct memseg *next;
    struct memseg *prev;
};
typedef struct memseg memseg_t;

// memory
struct memory {
    enum capacity_requirement requirement;
    uint32_t capacity;
    uint32_t used;
    int num_segments;
    memseg_t* segments;
};
typedef struct memory memory_t;


memory_t* memory_inf_init();
memory_t* memory_init(uint32_t capacity);
int allocate_memory(memory_t* mem, process_t* p);
int deallocate_memory(memory_t* mem, process_t* p);

#endif //PROJECT1_MEM_ALLOC_H
