/*
 * Author  : The Duy Nguyen - 1100548
 * File    : pseudo_process.h
 * Purpose : Header file for pseudo process C file.
 */
#ifndef PROJECT1_PSEUDO_PROCESS_H
#define PROJECT1_PSEUDO_PROCESS_H

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
    int arrival;
    int service_time;
    int time_left;
    int completed_time;
    int memory_requirement;
    enum status p_status;
};
typedef struct process process_t;


/* function prototypes */
process_t* process_init(const char *name, int request_time, int est_time, int memory);
int process_terminate(process_t *p);
int process_precede(process_t* tmp, process_t* pivot);
int process_exceed(process_t* tmp, process_t* pivot);

#endif //PROJECT1_PSEUDO_PROCESS_H
