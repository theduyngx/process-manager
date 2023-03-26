#include <stdio.h>
#include "sort.h"
#include <string.h>

#define SIZE 15

int main(void) {
    const char* names[SIZE] = {"a1sod", "sdf12", "a1sdo", "s", "a0c", "a1s",
                               "a2sod", "b", "a1sdo1", "a1sdo2", "g", "b1",
                               "ads", "a1sdp", "sdf15"};
    int tls[SIZE] = {1, 6, 12, 8, 12, 1, 1, 9, 12,
                     12, 5, 7, 10, 12, 1};
    int req[SIZE] = {0, 1, 2, 3, 4, 0, 0, 7, 2, 2,
                     10, 11, 12, 2, 0};
    process_t* parr[SIZE];
    printf("INIT:\n");
    for (int i=0; i < SIZE; i++) {
        parr[i] = process_init(names[i], req[i], tls[i], 2);
        const char* tabs = "\t";
        if (strlen(parr[i]->name) < 5) tabs = "\t\t";
        printf("%s%s %d %d\n", parr[i]->name, tabs, parr[i]->time_left, parr[i]->requested_time);
    }
    sort(parr, SIZE);
    for (int i=0; i < SIZE; i++) {
        const char* tabs = "\t";
        if (strlen(parr[i]->name) < 5) tabs = "\t\t";
        printf("%s%s %d %d\n", parr[i]->name, tabs, parr[i]->time_left, parr[i]->requested_time);
    }
    return 0;
}
