/*
 * Author  : The Duy Nguyen - 1100548
 * File    : pseudo_process.h
 * Purpose : Header file for pseudo process C file.
 */

#ifndef PROJECT1_PSEUDO_PROCESS_H
#define PROJECT1_PSEUDO_PROCESS_H

#include <stdint.h>

/* data structures */
// status
enum status {
    READY,
    RUNNING,
    FINISHED
};

// pseudo-process data structure
struct process {
    const char *name;
    uint32_t arrival;
    uint32_t service_time;
    uint32_t time_left;
    uint32_t completed_time;
    unsigned int memory_requirement;
    enum status p_status;
};
typedef struct process process_t;


/* function prototypes */
process_t* process_init(const char *name, uint32_t request_time, uint32_t est_time, unsigned int memory);
int process_terminate(process_t *p);
int process_precede(process_t* tmp, process_t* pivot);
int process_exceed(process_t* tmp, process_t* pivot);

#endif //PROJECT1_PSEUDO_PROCESS_H
