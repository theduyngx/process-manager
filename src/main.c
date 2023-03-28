#include "queue.h"
#include "heap.h"
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
    for (int i=0; i < SIZE; i++)
        parr[i] = process_init(names[i], req[i], tls[i], 2);


    ///
    const char* names2[4] = {"P1", "P2", "P5", "P4"};
    int tls2[4] = {50, 30, 80, 10};
    int req2[4] = {0, 10, 40, 50};
    process_t* parr2[4];
    for (int i=0; i < 4; i++)
        parr2[i] = process_init(names2[i], req2[i], tls2[i], 8);


    const char* names3[2] = {"P1", "P2"};
    int tls3[2] = {100, 100};
    int req3[2] = {0, 500};
    process_t* parr3[2];
    for (int i=0; i < 2; i++)
        parr3[i] = process_init(names3[i], req3[i], tls3[i], 2);

    SJF_scheduler_optimized(parr2, 4, 3);
    ///

    return 0;
}
