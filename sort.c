#include "sort.h"

/* insertion sort algorithm */
void insertion_sort(int *arr, int size) {
    for (int i=1; i < size; i++) {
        int key = arr[i];
        int j = i-1;
        while (key < arr[j] && j >= 0) {
            arr[j + 1] = arr[j];
            --j;
        }
        arr[j+1] = key;
    }
}

/* Hoare's partitioning for quicksort algorithm. */
int partition(int *arr, int lo, int hi) {
    int p = (hi - lo) / 2 + lo;
    int i = lo-1;
    int j = hi+1;
    for (;;) {
        do i++; while (arr[i] < p);
        do j--; while (arr[j] > p);
        if (i >= j) return j;
        int tmp = arr[i];
        arr[i]  = arr[j];
        arr[j]  = tmp;
    }
}

/* quicksort algorithm */
void quicksort(int *arr, int lo, int hi) {
    if (lo >= hi) return;
    int p = partition(arr, lo, hi);
    quicksort(arr, lo, p);
    quicksort(arr, p + 1, hi);
}

/* hybrid sort algorithm for any integer array */
void sort(int *arr, int size) {
    if (! arr) return;
    if (size <= INSERTION_THRESHOLD)
        return insertion_sort(arr, size);
    quicksort(arr, 0, size);
}