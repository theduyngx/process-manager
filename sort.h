/*
 * Author  : The Duy Nguyen - 1100548
 * File    : sort.h
 * Purpose : Header file for functions related to sorting an array of processes by
 *           service time required to finish the process.
 */

#include "pseudo_process.h"
#define INSERTION_THRESHOLD 10         // largest array size to use insertion sort

int left_partition(process_t* tmp, process_t* pivot);
int right_partition(process_t* tmp, process_t* pivot);
void sort(process_t* arr[], int size); // sort function prototype