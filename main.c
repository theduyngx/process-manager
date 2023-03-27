#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "queue.h"

int main(void) {
    const char* names[SIZE] = {"a1sod", "sdf12", "a1sdo", "s", "a0c", "a1s",
                               "a2sod", "b", "a1sdo1", "a1sdo2", "g", "b1",
                               "ads", "a1sdp", "sdf15"};
    int tls[SIZE] = {1, 6, 12, 8, 12, 1, 1, 9, 12,
                     12, 5, 7, 10, 12, 1};
    int req[SIZE+5] = {0, 1, 2, 3, 4, 0, 0, 7, 2, 2,
                     10, 11, 12, 2, 0};
    process_t* parr[SIZE];
    printf("INIT:\n");
    for (int i=0; i < SIZE; i++) {
        parr[i] = process_init(names[i], req[i], tls[i], 2);
        const char* tabs = "\t";
        if (strlen(parr[i]->name) < 5) tabs = "\t\t";
        printf("%s%s %d %d\n", parr[i]->name, tabs, parr[i]->time_left, parr[i]->requested_time);
    }
//    sort(parr, SIZE);
//    for (int i=0; i < SIZE; i++) {
//        const char* tabs = "\t";
//        if (strlen(parr[i]->name) < 5) tabs = "\t\t";
//        printf("%s%s %d %d\n", parr[i]->name, tabs, parr[i]->time_left, parr[i]->requested_time);
//    }


    ///
    heap_t* h = heap_init();
    for (int i=0; i < SIZE; i++) {
        heap_insert(h, parr[i]);
    }
    for (int i=0; i < h->size; i++) {
        printf("%d ", h->arr[i]->time_left);
    }
    int i=0;
    printf("\n");
    while (h->size > 0) {
        i++;
        process_t* p = extract_max(h);
        printf("%d ", p->time_left);
        free(p);
    }
    printf("\n%d", i);
    ///

    return 0;
}
