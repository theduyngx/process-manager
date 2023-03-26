/*
 * Author  : The Duy Nguyen - 1100548
 * File    : sort.h
 * Purpose : Header file for functions related to sorting an array of processes by
 *           service time required to finish the process.
 */

#include "pseudo_process.h"
#define INSERTION_THRESHOLD 10 // what is the largest array size to use insertion instead of quicksort

void sort(process_t* arr[], int size); // sort function prototype