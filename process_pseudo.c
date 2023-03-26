/*
 * Author  : The Duy Nguyen - 1100548
 * File    : pseudo_process.h
 * Purpose : Data structures and functions related to handling the pseudo-processes.
 */

#include <stdlib.h>
#include "pseudo_process.h"
#include "assert.h"


/* Initialize the process */
process_t* process_init(const char *name, int request_time, int est_time, int memory) {
    process_t* p = (process_t *) malloc(sizeof(process_t));
    p->name = name;
    p->requested_time = request_time;
    p->time_left = est_time;
    p->memory_requirement = memory;
    p->p_status = READY;
    assert(p);
    return p;
}

/* Terminate the process */
void process_terminate(process_t *p) {
    free(p);
    assert(!p);
}
