/*
 * Author  : The Duy Nguyen - 1100548
 * File    : scheduler.h
 * Purpose : Header file for process scheduling algorithm.
 */

#ifndef PROJECT1_SCHEDULER_H
#define PROJECT1_SCHEDULER_H


#include "mem_alloc.h"

/* SJF and RR */
void SJF_scheduler(queue_t* buffer, memory_t* mem, unsigned int quantum);
void RR_scheduler(queue_t* buffer, memory_t* mem, unsigned int quantum);


/* additional preemptive algorithm */
__attribute__((unused))
void SRTN_scheduler(queue_t* buffer, memory_t* mem, unsigned int quantum);


#endif //PROJECT1_SCHEDULER_H
