/*
 * Author  : The Duy Nguyen - 1100548
 * File    : process_real.c
 * Purpose : Header file for real processes. It is not responsible for creating, continuing,
 *           suspending, and terminating real processes.
 */

#ifndef PROJECT1_CLION_PROCESS_REAL_H
#define PROJECT1_CLION_PROCESS_REAL_H


#include "pseudo_process.h"

#define NUM_ENDIAN_BYTES 4

/* Handling real processes function prototypes */
int create_process(process_t* p, uint32_t timer);
int suspend_process(process_t* p, uint32_t timer);
int continue_process(process_t* p, uint32_t timer);
int terminate_process(process_t* p);

/* Big endian byte ordering conversion */
void big_endian(char order[NUM_ENDIAN_BYTES], uint32_t value);


#endif //PROJECT1_CLION_PROCESS_REAL_H
