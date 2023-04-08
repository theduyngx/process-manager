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


/**
 * Initialize a process.
 * @param name          process name
 * @param arrival       arrival time
 * @param service_time  service time (time taken for process to be completed)
 * @param size          memory requirement for process
 * @return              the initialized process
 */
process_t* process_init(const char *name, uint32_t arrival, uint32_t service_time, unsigned int size) {
    process_t* p = (process_t *) malloc(sizeof(process_t));
    p->name = strdup(name);
    p->arrival = arrival;
    p->service_time = service_time;
    p->time_left = p->service_time;
    p->completed_time = -1;
    p->size = size;
    assert(p);

    // not sure if we need this - but after this step we must read in the Big Endian byte ordering
    // probably pipe (using dup2)
    // recall that in dup2 we must use pipe to create a piped file descriptor, and use dup2 to read in
    // the stdin for a process and its stdout

    // apparently, in the specs it says we must read the 32 bit simulation time of when the process is
    // created, which is probably our uint32_t timer (the current time that is) - in Big Endian order

    // finally, we use fd[1] which is the piped stdout to read 1 byte (however that works) from it to
    // compare and make sure that it is identical to our stdin least significant bits.
    return p;
}


/**
 * Terminate a process.
 * @param p  given process
 * @return   0 if termination succeeds, or otherwise
 */
int process_terminate(process_t *p) {
    if (p == NULL) {
        fprintf(stderr, "Process %s is NULL!\n", p->name);
        exit(1);
    }
    if (p->status != FINISHED) {
        fprintf(stderr, "Process %s is not finished!\n", p->name);
        exit(3);
    }
    free(p);
    return 0;
}


/**
 * Check whether a given process strictly precedes another in terms of priority.
 * @param tmp   process to be checked
 * @param pivot process that's checked against
 * @return      0 (False) if not preceding, and 1 (True) if preceding
 */
int process_precede(process_t* tmp, process_t* pivot) {
    if (tmp == NULL || pivot == NULL) {
        fprintf(stderr, "ERROR - process_precede: null inputs\n");
        exit(1);
    }
    uint32_t tmp_time      = tmp->time_left;
    uint32_t tmp_request   = tmp->arrival;
    const char* tmp_name   = tmp->name;
    uint32_t pivot_time    = pivot->time_left;
    uint32_t pivot_request = pivot->arrival;
    const char* pivot_name = pivot->name;
    if (tmp_time == pivot_time) {
        if (tmp_request < pivot_request) return 1;
        else if (tmp_request == pivot_request)
            return (strcmp(tmp_name, pivot_name) < 0);
    }
    return (tmp_time < pivot_time);
}


/**
 * Check whether a given process strictly proceeds another in terms of priority.
 * @param tmp   process to be checked
 * @param pivot process that's checked against
 * @return      0 (False) if not proceeding, and 1 (True) if proceeding
 */
int process_exceed(process_t* tmp, process_t* pivot) {
    if (tmp == NULL || pivot == NULL) {
        fprintf(stderr, "ERROR - process_precede: null inputs\n");
        exit(1);
    }
    uint32_t tmp_time      = tmp->time_left;
    uint32_t tmp_request   = tmp->arrival;
    const char* tmp_name   = tmp->name;
    uint32_t pivot_time    = pivot->time_left;
    uint32_t pivot_request = pivot->arrival;
    const char* pivot_name = pivot->name;
    if (tmp_time == pivot_time) {
        if (tmp_request > pivot_request) return 1;
        else if (tmp_request == pivot_request)
            return (strcmp(tmp_name, pivot_name) > 0);
    }
    return (tmp_time > pivot_time);
}
