#include <stdio.h>
#include <string.h>
#include "input_queue.h"
#include "ready_queue.h"
#include "scheduler.h"
#include "sort.h"

int main(void) {
    const char* names[SIZE] = {"P0", "P1", "P2", "P3", "P4", "P5", "P6",
                               "P7", "P8", "P9", "P10", "P11", "P12",
                               "P13", "P14"};
    int tls[SIZE] = {15, 90, 57, 74, 27, 65, 52, 123, 85,
                     33, 50, 53, 63, 8, 103};
    int req[SIZE+5] = {0, 14, 16, 17, 42, 44, 46, 53, 62,
                       65, 70, 76, 77, 97, 100};
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
