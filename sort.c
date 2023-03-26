/*
 * Author  : The Duy Nguyen - 1100548
 * File    : sort.c
 * Purpose : Sorting an array of processes by service time required to finish the process.
 */

#include "sort.h"
#include <stdio.h>
#include <string.h>

/* Check whether a given process is before another */
int left_partition(process_t* tmp, process_t* pivot) {
    int tmp_time           = tmp->time_left;
    int tmp_request        = tmp->requested_time;
    const char* tmp_name   = tmp->name;
    int pivot_time         = pivot->time_left;
    int pivot_request      = pivot->requested_time;
    const char* pivot_name = pivot->name;
    if (tmp_time < pivot_time) return 1;
    else if (tmp_time == pivot_time) {
        if (tmp_request < pivot_request) return 1;
        else if (tmp_request == pivot_request)
            if (strcmp(tmp_name, pivot_name) < 0) return 1;
    }
    return 0;
}

/* Check whether a given process is after another */
int right_partition(process_t* tmp, process_t* pivot) {
    int tmp_time           = tmp->time_left;
    int tmp_request        = tmp->requested_time;
    const char* tmp_name   = tmp->name;
    int pivot_time         = pivot->time_left;
    int pivot_request      = pivot->requested_time;
    const char* pivot_name = pivot->name;
    if (tmp_time > pivot_time) return 1;
    else if (tmp_time == pivot_time) {
        if (tmp_request > pivot_request) return 1;
        else if (tmp_request == pivot_request)
            if (strcmp(tmp_name, pivot_name) > 0) return 1;
    }
    return 0;
}

/* Insertion sort algorithm */
void insertion_sort(process_t* arr[], int size) {
    for (int i=1; i < size; i++) {
        process_t* p = arr[i];
        int j = i-1;
        while (j >= 0 && right_partition(arr[j], p)) {
            arr[j+1] = arr[j];
            j--;
        }
        arr[j+1] = p;
    }
}

/* Hoare's partitioning for quicksort algorithm. */
int partition(process_t* arr[], int lo, int hi) {
    int mid = (hi - lo)/2 + lo;
    int i = lo-1;
    int j = hi+1;
    process_t* pivot = arr[mid];
    for (;;) {
        do i++; while (left_partition (arr[i], pivot));
        do j--; while (right_partition(arr[j], pivot));
        if (i >= j) return j;
        process_t* tmp = arr[i];
        arr[i] = arr[j];
        arr[j] = tmp;
    }
}

/* Quicksort algorithm */
void quicksort(process_t* arr[], int lo, int hi) {
    if (lo >= hi) return;
    int p = partition(arr, lo, hi);
    quicksort(arr, lo, p);
    quicksort(arr, p + 1, hi);
}

/* Hybrid sort algorithm for a process array, sorted by service time required to finish process */
void sort(process_t* arr[], int size) {
    if (! arr) return;
    if (size <= INSERTION_THRESHOLD) {
        printf("\nINSERTION SORT USED:\n");
        return insertion_sort(arr, size);
    }
    printf("\nQUICKSORT USED:\n");
    quicksort(arr, 0, size-1);
}