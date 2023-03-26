#include <stdio.h>
#include "sort.h"

#define SIZE 15

int main(void) {
    int tls[SIZE] = {1, 6, 12, 8, 12, 1, 1, 9, 12,
                     12, 5, 7, 10, 12, 1};
    process_t* parr[SIZE];
    printf("INIT:\n");
    for (int i=0; i < SIZE; i++) {
        parr[i] = process_init("process", i, tls[i], 2);
        printf("%d %d \n", parr[i]->time_left, parr[i]->requested_time);
    }
    sort(parr, SIZE);
    for (int i=0; i < SIZE; i++) {
        printf("%d %d \n", parr[i]->time_left, parr[i]->requested_time);
    }
    return 0;
}
