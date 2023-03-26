/*
 * Author  : The Duy Nguyen - 1100548
 * File    : sort.c
 * Purpose : Sorting an array of processes by service time required to finish the process.
 */

#include "sort.h"

/* insertion sort algorithm */
void insertion_sort(process_t* arr[], int size) {
    for (int i=1; i < size; i++) {
        process_t* p = arr[i];
        int key = p->time_left;
        int j = i-1;
        while (j >= 0 && key < arr[j]->time_left) {
            arr[j+1] = arr[j];
            j--;
        }
        arr[j+1] = p;
    }
}

/* Hoare's partitioning for quicksort algorithm. */
int partition(process_t* arr[], int lo, int hi) {
    int mid = (hi - lo) / 2 + lo;
    int i = lo-1;
    int j = hi+1;
    int p = arr[mid]->time_left;
    for (;;) {
        do i++; while (arr[i]->time_left < p);
        do j--; while (arr[j]->time_left > p);
        if (i >= j) return j;
        process_t* tmp = arr[i];
        arr[i] = arr[j];
        arr[j] = tmp;
    }
}

/* quicksort algorithm */
void quicksort(process_t* arr[], int lo, int hi) {
    if (lo >= hi) return;
    int p = partition(arr, lo, hi);
    quicksort(arr, lo, p);
    quicksort(arr, p + 1, hi);
}

/* hybrid sort algorithm for a process array, sorted by service time required to finish process */
void sort(process_t* arr[], int size) {
    if (! arr) return;
    if (size <= INSERTION_THRESHOLD)
        return insertion_sort(arr, size);
    quicksort(arr, 0, size-1);
}