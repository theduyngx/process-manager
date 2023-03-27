/*
 * Author  : The Duy Nguyen - 1100548
 * File    : sort.h
 * Purpose : Header file related to sorting an array of processes by (in this order of priority) service
 *           time required to finish the process, then request time, then lexicographical order.
 */

#include "pseudo_process.h"
#define INSERTION_THRESHOLD 10         // largest array size to use insertion sort

void sort(process_t* arr[], int size); // sort function prototype