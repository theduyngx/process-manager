/*
 * Author  : The Duy Nguyen - 1100548
 * File    : scheduler.h
 * Purpose : Header file for process scheduling algorithm.
 *           Also including helper function prototypes.
 */

#ifndef PROJECT1_SCHEDULER_H
#define PROJECT1_SCHEDULER_H


#include "mem_alloc.h"
#include "ready_queue.h"
#include "process_real.h"

/* Nonpreemptive schedulers */
__attribute__((unused)) __attribute__((unused))
void FCFS_scheduler(queue_t* buffer, memory_t* mem, unsigned int quantum);
void SJF_scheduler(queue_t* buffer, memory_t* mem, unsigned int quantum);


/* Preemptive schedulers */
__attribute__((unused))
void SRTN_scheduler(queue_t* buffer, memory_t* mem, unsigned int quantum);
void RR_scheduler(queue_t* buffer, memory_t* mem, unsigned int quantum);


/* Preemption-based scheduling algorithm templates */
void nonpreemptive_scheduler(queue_t* buffer, memory_t* mem, unsigned int quantum, enum structure rq_type);
void preemptive_scheduler(queue_t* buffer, memory_t* mem, unsigned int quantum, enum structure rq_type);


/* Helper functions */
void update_queues(queue_t* buffer, memory_t* mem, queue_t* input_queue, ready_queue_t* ready_queue,
                   uint32_t timer);
void clear_buffer(process_t* buffer[], int size);
void print_ready(uint32_t timer, process_t* p, unsigned int base);
void print_running(uint32_t timer, process_t* p);
void print_finished(uint32_t timer, process_t* p, int proc_remaining, char sha[NUM_SHA_BYTES+1]);
void print_statistics(process_t* all_processes[], int num_process, uint32_t makespan);
void finish_process(process_t** running, memory_t* mem, process_t** finished, int *index,
                    uint32_t timer, int proc_remaining);


#endif //PROJECT1_SCHEDULER_H
