
#include <stdio.h>
#include <stdlib.h>
#include "input_queue.h"
#include "ready_queue.h"
#include "scheduler.h"

void SJB_scheduler(process_t* buffer[], int size, int quantum) {
    input_t* inputs = input_init();
    ready_t* ready_queue = ready_init();
    int i = 0;
    int timer = 0;
    int num_quantum = 0;
    int fill_quantum = 0;

    process_t* running = NULL;
    int condition = 1;
    while (i < size || condition) {
        // while all processes in buffer arrive within a quantum, we enqueue them to input queue
        while (i < size && buffer[i]->arrival <= timer) {
            process_t* p = buffer[i];
            enqueue(inputs, p);
            i++;
        }
        input_t* tmp = inputs;
        // we iterate through input queue and insert each process to the ready queue
        for (int j=0; j < i; j++) {
            input_t* input = dequeue(&inputs);
            process_t* p = input->process;
            free(input);
            ready_insert(ready_queue, p);
            tmp = tmp->next;
        }
        // now we run shortest job process by decrementing quantum each time
        if (running == NULL) {
            running = extract_max(ready_queue);
            printf("%d,RUNNING,process_name=%s,remaining_time=%d\n",
                   timer, running->name, running->time_left);
        }
        if (running->time_left > fill_quantum) {
            timer += quantum - fill_quantum;
            running->time_left -= fill_quantum;
            num_quantum++;
        }
        // if job has been finished
        else if (running->time_left < quantum) {
            timer += running->time_left;
            fill_quantum = (fill_quantum + 1) % 3;
            printf("%d,FINISHED,process_name=%s,proc_remaining=%d\n",
                   timer, running->name, ready_queue->size);
            process_t* sth = running;
            free(sth);
            running = NULL;
        }
        else {
            running->time_left -= quantum;
            timer += quantum;
            num_quantum++;
        }
        condition = ready_queue->size > 0;
    }
}
