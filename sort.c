/*
 * Author  : The Duy Nguyen - 1100548
 * File    : sort.c
 * Purpose : Sorting an array of processes by service time required to finish the process.
 */

#include "sort.h"
#include <stdio.h>

int left_partition(int tmp_time, int tmp_request, int pivot_time, int pivot_request) {
    return tmp_time < pivot_time || (tmp_time == pivot_time && tmp_request < pivot_request);
}
int right_partition(int tmp_time, int tmp_request, int pivot_time, int pivot_request) {
    return tmp_time > pivot_time || (tmp_time == pivot_time && tmp_request > pivot_request);
}

/* Insertion sort algorithm */
void insertion_sort(process_t* arr[], int size) {
    for (int i=1; i < size; i++) {
        process_t* p = arr[i];
        int p_time = p->time_left;
        int p_request = p->requested_time;
        int j = i-1;
        while (j >= 0 && right_partition(arr[j]->time_left, arr[j]->requested_time,
                                         p_time, p_request)) {
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
    int pivot_time = arr[mid]->time_left;
    int pivot_request = arr[mid]->requested_time;
    for (;;) {
        do i++; while (left_partition (arr[i]->time_left, arr[i]->requested_time,
                                       pivot_time, pivot_request));
        do j--; while (right_partition(arr[j]->time_left, arr[j]->requested_time,
                                       pivot_time, pivot_request));
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