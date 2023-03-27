/*
 * Author  : The Duy Nguyen - 1100548
 * File    : pseudo_process.h
 * Purpose : Header file for pseudo process C file.
 */


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
    int requested_time;
    int time_left;
    int memory_requirement;
    enum status p_status;
};
typedef struct process process_t;


/* function prototypes */
process_t* process_init(const char *name, int request_time, int est_time, int memory);
int process_terminate(process_t *p);
