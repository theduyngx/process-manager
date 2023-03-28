/*
 * Author  : The Duy Nguyen - 1100548
 * File    : scheduler.h
 * Purpose : Header file for process scheduling algorithm.
 */

#ifndef PROJECT1_SCHEDULER_H
#define PROJECT1_SCHEDULER_H

/* no optimization */
void SJF_scheduler(process_t* buffer[], int size, unsigned int quantum);
void RR_scheduler(process_t* buffer[], int size, unsigned int quantum);


/* improved / optimized versions */
__attribute__((unused))
void SJF_scheduler_optimized(process_t* buffer[], int size, unsigned int quantum);

__attribute__((unused))
void SRTN_scheduler(process_t* buffer[], int size, unsigned int quantum);

#endif //PROJECT1_SCHEDULER_H
