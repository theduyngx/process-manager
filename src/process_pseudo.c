/*
 * Author  : The Duy Nguyen - 1100548
 * File    : pseudo_process.c
 * Purpose : Functions related to pseudo-processes.
 */

#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "pseudo_process.h"


/* Initialize the process */
process_t* process_init(const char *name, int arrival, int est_time, int memory) {
    process_t* p = (process_t *) malloc(sizeof(process_t));
    p->name = name;
    p->arrival = arrival;
    p->time_left = est_time;
    p->memory_requirement = memory;
    p->p_status = READY;
    assert(p);
    return p;
}

/* Terminate the process */
int process_terminate(process_t *p) {
    if (p->p_status != FINISHED) return 1;
    free(p);
    assert(!p);
    return 0;
}


/* Check whether a given process is before another */
int process_precede(process_t* tmp, process_t* pivot) {
    int tmp_time           = tmp->time_left;
    int tmp_request        = tmp->arrival;
    const char* tmp_name   = tmp->name;
    int pivot_time         = pivot->time_left;
    int pivot_request      = pivot->arrival;
    const char* pivot_name = pivot->name;
    if (tmp_time < pivot_time) return 1;
    else if (tmp_time == pivot_time) {
        if (tmp_request < pivot_request) return 1;
        else if (tmp_request == pivot_request)
            if (strcmp(tmp_name, pivot_name) < 0) return 1;
    }
    return 0;
}

/* Check whether a given process is after another */
int process_exceed(process_t* tmp, process_t* pivot) {
    int tmp_time           = tmp->time_left;
    int tmp_request        = tmp->arrival;
    const char* tmp_name   = tmp->name;
    int pivot_time         = pivot->time_left;
    int pivot_request      = pivot->arrival;
    const char* pivot_name = pivot->name;
    if (tmp_time > pivot_time) return 1;
    else if (tmp_time == pivot_time) {
        if (tmp_request > pivot_request) return 1;
        else if (tmp_request == pivot_request)
            if (strcmp(tmp_name, pivot_name) > 0) return 1;
    }
    return 0;
}
