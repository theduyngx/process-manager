/// CHECK: for whether process size exceeds capacity and terminate it altogether
/// --> avoid infinite loop
#include <stdio.h>

#include <stdlib.h>
#include <assert.h>
#include "mem_alloc.h"
#include "pseudo_process.h"


memory_t* memory_inf_init() {
    memory_t* mem = (memory_t*) malloc(sizeof(memory_t));
    assert(mem);
    mem->requirement = INF;
    mem->capacity = 0xFFFFFFFF;
    return mem;
}

memory_t* memory_init(uint32_t capacity) {
    memory_t* mem = (memory_t*) malloc(sizeof(memory_t));
    assert(mem);
    mem->requirement = FIN;
    mem->capacity = capacity;
    mem->used = 0;
    mem->segments = (memseg_t*) malloc(sizeof(memseg_t));
    assert(mem->segments);
    mem->num_segments = 1;
    mem->segments->size = capacity;
    mem->segments->state = HOLE;
    return mem;
}


int allocate_memory(memory_t* mem, process_t* p) {
    // if infinite memory
    if (mem->requirement == INF) return SUCCESS;

    // if process size cannot possibly fit memory
    uint32_t p_size = p->size;
    if (mem->used + p_size > mem->capacity) return FAILURE;

    // find closest best fit
    int allocated = FAILURE;
    uint32_t min_size = 0xFFFFFFFF;
    memseg_t* seg = mem->segments;
    memseg_t* min_seg = seg;
    for (int i=0; i < mem->num_segments; i++) {

        // if process size fits into current segment size then process can be allocated
        if (p_size <= seg->size) allocated = SUCCESS;
        if (min_size < seg->size) {
            min_size = seg->size;
            min_seg = seg;
        }
        seg = seg->next;
    }

    // if not able to allocate
    if (allocated == FAILURE) return FAILURE;

    // allocate best fit memory to process
    uint32_t diff = min_size - p_size;
    mem->used += p_size;
    assert(diff >= 0 && mem->used < mem->capacity);
    (mem->num_segments)++;
    min_seg->state = PROCESS;
    min_seg->size = p_size;
    min_seg->p_address = &p;
    if (diff > 0) {
        // create a new hole
        memseg_t* hole = (memseg_t*) malloc(sizeof(memseg_t));
        hole->state = HOLE;
        hole->size = diff;
        hole->prev = min_seg;
        hole->next = min_seg->next;
        min_seg->next = hole;
    }
    return SUCCESS;
}


int deallocate_memory(memory_t* mem, process_t* p) {
    if (mem->requirement == INF) return SUCCESS;
    if (p->time_left > 0) return FAILURE;
    // find process in memory
    int found = FAILURE;
    memseg_t* seg = mem->segments;
    for (int i=0; i < mem->num_segments; i++) {

        ///
        printf("FINE\n");
        ///

        if (seg->p_address == &p) {
            found = SUCCESS;

            // making it a hole
            seg->state = HOLE;
            free(seg->p_address);

            // merging
            memseg_t* next = seg->next;
            memseg_t* prev = seg->prev;
            if (next != NULL && next->state == HOLE) {
                seg->size += next->size;
                seg->next = next->next;
                seg->next->prev = seg;
                (mem->num_segments)--;
                free(next);
            }
            if (prev != NULL && prev->state == HOLE) {
                prev->size += seg->size;
                prev->next = seg->next;
                seg->next->prev = prev;
                (mem->num_segments)--;
                free(seg);
            }
            break;
        }
        seg = seg->next;
    }
    assert(mem->num_segments > 0);
    return found;
}
