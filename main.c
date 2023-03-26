#include <stdio.h>
#include <stdlib.h>
#include "sort.h"

int main(void) {
    int arr[7] = {4, 6, 2, 8, 9, 1, 1};
    process_t* parr[7];
    for (int i=0; i < 7; i++) {
        parr[i] = process_init("process", i, arr[i], 2);
        printf("%d ", parr[i]->time_left);
    }
    sort(parr, 7);
    printf("\nSORTED:\n");
    for (int i=0; i < 7; i++) {
        printf("%d ", parr[i]->time_left);
    }
    return 0;
}
