/*
 * Author  : The Duy Nguyen - 1100548
 * File    : mem_alloc.c
 * Purpose : File with functions related to creating memory and allocating memory for processes.
 *
 * PROBLEM: Infinite memory not able to allocate memory sometimes (???)
 */

#include <stdlib.h>
#include <assert.h>
#include "mem_alloc.h"
#include "pseudo_process.h"


/* initialize memory segment / block */
memseg_t* memseg_init(unsigned int size, unsigned int base, enum segment_state state) {
    memseg_t* memseg = (memseg_t*) malloc(sizeof(memseg_t));
    assert(memseg);
    memseg->size = size;
    memseg->base = base;
    memseg->state = state;
    return memseg;
}

/* initialize logical memory with limited capacity */
memory_t* memory_init(unsigned int capacity) {
    memory_t* mem = (memory_t*) malloc(sizeof(memory_t));
    assert(mem);
    mem->requirement = FIN;
    mem->capacity = capacity;
    mem->used = 0;
    mem->segments = memseg_init(capacity, 0, HOLE);
    mem->num_segments = 1;
    return mem;
}

/* initialize logical memory with infinite memory */
memory_t* memory_inf_init() {
    memory_t* mem = memory_init(0xffffffff);
    assert(mem);
    assert(mem->segments);
    mem->requirement = INF;
    return mem;
}


/* allocate memory for process;
 * returns SUCCESS (0) or FAILURE (1) to indicate whether allocation succeeds or not
 */
int allocate_memory(memory_t* mem, process_t* p, unsigned int* base) {
    // if process is too large for memory then kill
    unsigned int p_size = p->size;
    if (p_size > mem->capacity) {
        p->p_status = FINISHED;
        process_terminate(p);
    }
    // else if process cannot presently fit memory
    else if (mem->used + p_size > mem->capacity) return FAILURE;

    // find closest best fit
    int allocated = FAILURE;
    unsigned int min_size = FINITE_CAPACITY;
    memseg_t* seg = mem->segments;
    memseg_t* min_seg = seg;
    for (int i=0; i < mem->num_segments; i++) {

        // if process size fits into current segment size then process can be allocated
        if (seg->state == HOLE) {
            if (p_size <= seg->size) allocated = SUCCESS;
            if (seg->size < min_size) {
                min_size = seg->size;
                min_seg = seg;
            }
        }
        seg = seg->next;
    }

    // if not able to allocate
    if (allocated == FAILURE) {
        assert(mem->requirement == FIN);
        return FAILURE;
    }

    // assert process fits and memory used does not exceed capacity
    unsigned int diff = min_size - p_size;
    assert(diff >= 0);
    mem->used += p_size;
    *base = min_seg->base;
    assert(mem->used <= mem->capacity);

    // allocate best fit memory to process
    min_seg->state = PROCESS;
    min_seg->size = p_size;
    min_seg->process = p;

    if (diff > 0) {
        // create a new hole
        unsigned int hole_base = (*base) + p_size;
        assert(hole_base + diff <= mem->capacity);       // assert not exceeding capacity
        memseg_t* hole = memseg_init(diff, hole_base, HOLE);
        hole->prev = min_seg;
        hole->next = min_seg->next;
        min_seg->next = hole;
        assert(hole->prev && hole);                 // assert hole exists properly
        (mem->num_segments)++;
    }
    return SUCCESS;
}


/* deallocate memory of process;
 * returns SUCCESS (0) or FAILURE (1) to indicate whether de-allocation succeeds or not
 */
int deallocate_memory(memory_t* mem, process_t* p) {
    if (mem->requirement == INF) return SUCCESS;
    if (p->time_left > 0) return FAILURE;

    // find process in memory
    int found = FAILURE;
    memseg_t* seg = mem->segments;
    for (int i=0; i < mem->num_segments; i++) {
        if (seg->state == PROCESS && seg->process == p) {
            found = SUCCESS;

            // making it a hole
            seg->state = HOLE;
            mem->used -= seg->process->size;
            seg->process = NULL;

            // merging proceeding block
            memseg_t* next = seg->next;
            if (next != NULL && next->state == HOLE) {
                seg->size += next->size;
                seg->next = next->next;

                // next is not at rear of memory
                if (next->next != NULL)
                    seg->next->prev = seg;
                (mem->num_segments)--;
                free(next);
            }

            // merging preceding block
            memseg_t* prev = seg->prev;
            if (prev != NULL && prev->state == HOLE) {
                prev->size += seg->size;
                prev->next = seg->next;

                // seg is not at rear of memory
                if (seg->next != NULL)
                    prev->next->prev = prev;
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


/* free memory data structure */
void free_memory(memory_t* mem) {
    while (mem->num_segments > 1) {
        mem->segments = mem->segments->next;
        free(mem->segments->prev);
        (mem->num_segments)--;
    }
    free(mem->segments);
    free(mem);
}
