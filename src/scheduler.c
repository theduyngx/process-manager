
#include <stdio.h>
#include <stdlib.h>
#include "input_queue.h"
#include "ready_queue.h"
#include "scheduler.h"

void SJF_scheduler(process_t* buffer[], int size, int quantum) {
    input_t* inputs = input_init();
    ready_t* ready_queue = ready_init();
    int i = 0;
    int timer = 0;
    int num_quantum = 0;
    int fill_quantum = quantum;

    process_t* running = NULL;
    int condition = 1;
    while (i < size || condition) {
        ///
        // printf("timer = %d\n", timer);
        // printf("i (before while) = %d\n", i);
        ///

        // while all processes in buffer arrive within a quantum, we enqueue them to input queue
        // NOTE: modulo operation implies we only check this after a quantum has fully elapsed
        int start = i;
        while (i < size && timer % quantum == 0 && buffer[i]->arrival <= timer) {
            process_t* p = buffer[i];
            enqueue(inputs, p);
            i++;
            ///
            // printf("%d %d %d\n", i, p->arrival, p->time_left);
            ///
        }
        int end = i;
        // we iterate through input queue and insert each process to the ready queue
        for (int j=0; j < end-start; j++) {
            input_t* input = dequeue(&inputs);
            process_t* p = input->process;
            free(input);
            ready_insert(ready_queue, p);
        }
        ///
        // printf("FINE\n");
        ///
        // now we run shortest job process by decrementing quantum each time
        if (running == NULL) {
            running = extract_max(ready_queue);
            if (running != NULL)
                printf("%d,RUNNING,process_name=%s,remaining_time=%d\n",
                   timer, running->name, running->time_left);
        }
        if (running != NULL) {
            // fill up rest of quantum
            // fill quantum is the hole in the quantum yet to be filled
            if (fill_quantum != quantum && running->time_left > fill_quantum) {
                timer += fill_quantum;
                fill_quantum = quantum;
                running->time_left -= fill_quantum;
                num_quantum++;
            }
            // job finishes within given quantum
            else if (running->time_left <= fill_quantum) {
                timer += running->time_left;
                fill_quantum -= running->time_left;
                ///
                // printf("fill quantum = %d\n", fill_quantum);
                ///
                printf("%d,FINISHED,process_name=%s,proc_remaining=%d\n",
                       timer, running->name, ready_queue->size);
                process_t* tmp = running;
                free(tmp);
                running = NULL;
            }
            else {
                fill_quantum = quantum;
                running->time_left -= quantum;
                timer += quantum;
                num_quantum++;
            }
        }
        else {
            timer += fill_quantum;
            fill_quantum = quantum;
        }
        condition = ready_queue->size > 0;
    }
}
