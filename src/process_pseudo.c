/*
 * Author  : The Duy Nguyen - 1100548
 * File    : pseudo_process.c
 * Purpose : Functions related to pseudo-processes.
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "pseudo_process.h"


/* Initialize the process */
process_t* process_init(const char *name, uint32_t arrival, uint32_t service_time, unsigned int size) {
    process_t* p = (process_t *) malloc(sizeof(process_t));
    p->name = name;
    p->arrival = arrival;
    p->service_time = service_time;
    p->time_left = p->service_time;
    p->completed_time = -1;
    p->size = size;
    assert(p);
    return p;
}

/* Terminate the process */
int process_terminate(process_t *p) {
    assert(p);
    if (p->status != FINISHED) {
        printf("Process %s is not finished!\n", p->name);
        return 1;
    }
    free(p);
    return 0;
}


/* Check whether a given process is before another */
int process_precede(process_t* tmp, process_t* pivot) {
    uint32_t tmp_time      = tmp->time_left;
    uint32_t tmp_request   = tmp->arrival;
    const char* tmp_name   = tmp->name;
    uint32_t pivot_time    = pivot->time_left;
    uint32_t pivot_request = pivot->arrival;
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
    uint32_t tmp_time      = tmp->time_left;
    uint32_t tmp_request   = tmp->arrival;
    const char* tmp_name   = tmp->name;
    uint32_t pivot_time    = pivot->time_left;
    uint32_t pivot_request = pivot->arrival;
    const char* pivot_name = pivot->name;
    if (tmp_time > pivot_time) return 1;
    else if (tmp_time == pivot_time) {
        if (tmp_request > pivot_request) return 1;
        else if (tmp_request == pivot_request)
            if (strcmp(tmp_name, pivot_name) > 0) return 1;
    }
    return 0;
}
