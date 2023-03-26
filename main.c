#include <stdio.h>
#include <stdlib.h>
#include "sort.h"

int main(void) {
    int size = 4;
    int *arr = (int *) malloc(sizeof(int) * size);
    arr[0] = 5; arr[1] = 9; arr[2] = 3; arr[3] = 1; arr[4] = 1;
    for (int i=0; i <= size; i++) {
        printf("%d", arr[i]);
    }
    sort(arr, size);
    printf("\n");
    for (int i=0; i <= size; i++) {
        printf("%d", arr[i]);
    }
    return 0;
}
