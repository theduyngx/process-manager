/*
 * Author  : The Duy Nguyen - 1100548
 * File    : sort.c
 * Purpose : Sorting an array of processes by (in this order of priority) service time required
 *           to finish the process, then request time, then lexicographical order.
 */

#include "sort.h"
#include <stdio.h>

/* Insertion sort algorithm */
void insertion_sort(process_t* arr[], int size) {
    for (int i=1; i < size; i++) {
        process_t* p = arr[i];
        int j = i-1;
        while (j >= 0 && process_exceed(arr[j], p)) {
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
        do i++; while (process_precede(arr[i], pivot));
        do j--; while (process_exceed (arr[j], pivot));
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