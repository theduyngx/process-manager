#include <stdio.h>
#include <string.h>
#include "input_queue.h"
#include "ready_queue.h"
#include "scheduler.h"
#include "sort.h"

int main(void) {
    const char* names[SIZE] = {"a1sod", "sdf12", "a1sdo", "s", "a0c", "a1s",
                               "a2sod", "b", "a1sdo1", "a1sdo2", "g", "b1",
                               "ads", "a1sdp", "sdf15"};
    int tls[SIZE] = {1, 6, 12, 8, 12, 1, 1, 9, 12,
                     12, 5, 7, 10, 12, 1};
    int req[SIZE+5] = {0, 0, 0, 1, 2, 3, 4, 4, 4,
                       6, 10, 15, 44, 50, 55};
    process_t* parr[SIZE];
    printf("INIT:\n");
    for (int i=0; i < SIZE; i++) {
        parr[i] = process_init(names[i], req[i], tls[i], 2);
        const char* tabs = "\t";
        if (strlen(parr[i]->name) < 5) tabs = "\t\t";
        printf("%s%s %d %d\n", parr[i]->name, tabs, parr[i]->time_left, parr[i]->arrival);
    }


    ///
    SJF_scheduler(parr, SIZE, 3);
    ///

    return 0;
}
