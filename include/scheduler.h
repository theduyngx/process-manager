/*
 * Author  : The Duy Nguyen - 1100548
 * File    : scheduler.h
 * Purpose : Header file for process scheduling algorithm.
 */

#ifndef PROJECT1_SCHEDULER_H
#define PROJECT1_SCHEDULER_H


#include "mem_alloc.h"

/* SJF and RR */
void SJF_scheduler(process_t* buffer[], int size, memory_t* mem, unsigned int quantum);
void RR_scheduler(process_t* buffer[], int size, memory_t* mem, unsigned int quantum);


/* additional algorithms */
__attribute__((unused))
void SJF_scheduler_optimized(process_t* buffer[], int size, unsigned int quantum);

__attribute__((unused))
void SRTN_scheduler(process_t* buffer[], int size, memory_t* mem, unsigned int quantum);


#endif //PROJECT1_SCHEDULER_H
