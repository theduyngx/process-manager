/// CHECK: for whether process size exceeds capacity and terminate it altogether
/// --> avoid infinite loop
#include <stdio.h>
#include <string.h>

#include <stdlib.h>
#include <assert.h>
#include "mem_alloc.h"
#include "pseudo_process.h"


memory_t* memory_inf_init() {
    memory_t* mem = (memory_t*) malloc(sizeof(memory_t));
    assert(mem);
    mem->requirement = INF;
    return mem;
}

memory_t* memory_init(unsigned int capacity) {
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
//    memory_t* mem = *memory;

    ///
//    printf("\nBEFORE MEMORY ALLOCATION ------------\n");
//    printf("Number of segments = %d\n", mem->num_segments);
//    for (int i=0; i < mem->num_segments; i++) {
//        printf("Segment %d - size = %d\n", i, mem->segments->size);
//    }
    ///

    // if infinite memory
    if (mem->requirement == INF) return SUCCESS;

    // if process size cannot possibly fit memory
    unsigned int p_size = p->size;
    if (mem->used + p_size > mem->capacity) return FAILURE;

    // find closest best fit
    int allocated = FAILURE;
    unsigned int min_size = MAX_CAPACITY;
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
    if (allocated == FAILURE) return FAILURE;

    // assert process fits and memory used does not exceed capacity
    unsigned int diff = min_size - p_size;
    assert(diff >= 0);

    ///
//    printf("min_size = %d\n", min_size);
//    printf("p_size = %d\n", p_size);
//    printf("diff = %d\n", diff);
//    printf("mem->used (before) = %d\n", mem->used);
    ///

    mem->used += p_size;
    assert(mem->used < mem->capacity);

    ///
//    printf("mem->used (after) = %d\n", mem->used);
    ///

    // allocate best fit memory to process
    min_seg->state = PROCESS;
    min_seg->size = p_size;
    min_seg->process = p;

    ///
//    printf("min_seg process name is %s\n", min_seg->process->name);
    ///

    if (diff > 0) {
        // create a new hole
        memseg_t* hole = (memseg_t*) malloc(sizeof(memseg_t));
        hole->state = HOLE;
        hole->size = diff;
        hole->prev = min_seg;
        hole->next = min_seg->next;
        min_seg->next = hole;
        assert(hole->prev && hole);
        (mem->num_segments)++;
    }

    ///
//    memseg_t* wev = mem->segments;
//    const char* str = (wev->process) ? wev->process->name : "";
//    printf("\nMEMORY ALLOCATION FOR %s ------------\n", str);
//    printf("Number of segments = %d\n", mem->num_segments);
//    for (int i=0; i < mem->num_segments; i++) {
//        printf("Segment %d - size = %d, state = %d\n", i, wev->size, wev->state);
//        if (wev->process) {
//            printf("Segment %d - name = %s, %d\n", i, wev->process->name, wev->process->size);
//        }
//        wev = wev->next;
//    }
    ///
//    *memory = mem;

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
//        if (strcmp(p->name, "P1") == 0) {
//            printf("%d %p %p\n", seg->state, seg->process, p);
//        }
        ///

        if (seg->state == PROCESS && seg->process == p) {
            found = SUCCESS;

            // making it a hole
            seg->state = HOLE;
            seg->process = NULL;

            // merging
            memseg_t* next = seg->next;
            memseg_t* prev = seg->prev;
            if (next != NULL && next->state == HOLE) {
                seg->size += next->size;
                seg->next = next->next;
                // next is not at rear of memory
                if (next->next != NULL)
                    seg->next->prev = seg;
                (mem->num_segments)--;
                free(next);
            }

            ///
//            printf("FINE\n");
            ///

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
