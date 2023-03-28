
#include <stdio.h>
#include <stdlib.h>
#include "input_queue.h"
#include "ready_queue.h"
#include "scheduler.h"


void print_running(int timer, const char* name, int queue_size) {
    printf("%d,RUNNING,process_name=%s,remaining_time=%d\n", timer, name, queue_size);
}

void print_finished(int timer, const char* name, int queue_size) {
    printf("%d,FINISHED,process_name=%s,proc_remaining=%d\n", timer, name, queue_size);
}

void print_statistics(process_t* buffer[], int num_process) {
    double total_turnaround = 0;
    double total_overhead = 0;
    double max_overhead = 0;
    for (int i=0; i < num_process; i++) {
        process_t* p = buffer[i];
        int turnaround = p->completed_time - p->arrival;
        total_turnaround += turnaround;
        double overhead = (double) turnaround / p->service_time;
        total_overhead += overhead;
        if (overhead > max_overhead) max_overhead = overhead;

        // FOR NOW: free processes here
        free(p);
    }
    double turnaround_double = total_turnaround / num_process;
    int turnaround_floor = (int) total_turnaround / num_process;
    int turnaround = turnaround_floor + (turnaround_floor < turnaround_double);
    printf("Turnaround time %d\n", turnaround);
    double average_overhead = total_overhead / num_process;
    printf("Time overhead %2f %2f", max_overhead, average_overhead);
}

void free_all_processes();


void SJF_scheduler(process_t* buffer[], int size, int quantum) {
    input_t* inputs = input_init();
    ready_t* ready_queue = ready_init();
    int i = 0;
    int timer = 0;
    int num_quantum = 0;

    process_t* running = NULL;
    int condition = 1;
    while (i < size || condition || running != NULL) {

        // while all processes in buffer arrive within a quantum, we enqueue them to input queue
        // NOTE: modulo operation implies we only check this after a quantum has fully elapsed
        int start = i;
        while (i < size && timer % quantum == 0 && buffer[i]->arrival <= timer) {
            process_t* p = buffer[i];
            enqueue(inputs, p);
            i++;
        }
        int end = i;
        // we iterate through input queue and insert each process to the ready queue
        for (int j=0; j < end-start; j++) {
            input_t* input = dequeue(&inputs);
            process_t* p = input->process;
            free(input);
            ready_insert(ready_queue, p);
        }
        // now we run shortest job process by decrementing quantum each time
        if (running == NULL) {
            running = extract_max(ready_queue);
            if (running != NULL) {
                running->p_status = RUNNING;
                print_running(timer, running->name, running->time_left);
            }
            else {
                timer += quantum;
                num_quantum++;
            }
        }
        else {
            // job finishes within given quantum
            if (running->time_left <= quantum) {
                timer += quantum;
                print_finished(timer, running->name, running->time_left);
                running->p_status = FINISHED;
                running->completed_time = timer;
                running = NULL;
            }
            else {
                running->time_left -= quantum;
                timer += quantum;
                num_quantum++;
            }
        }
        condition = ready_queue->size > 0;
    }
    // free memory
    free(inputs);
    free(ready_queue);
    print_statistics(buffer, size);
}


void SJF_scheduler_optimized(process_t* buffer[], int size, int quantum) {
    input_t* inputs = input_init();
    ready_t* ready_queue = ready_init();
    int i = 0;
    int timer = 0;
    int num_quantum = 0;
    // fill quantum is the hole in the quantum yet to be filled
    int fill_quantum = quantum;

    process_t* running = NULL;
    int condition = 1;
    while (i < size || condition || running != NULL) {

        // while all processes in buffer arrive within a quantum, we enqueue them to input queue
        // NOTE: modulo operation implies we only check this after a quantum has fully elapsed
        int start = i;
        while (i < size && timer % quantum == 0 && buffer[i]->arrival <= timer) {
            process_t* p = buffer[i];
            enqueue(inputs, p);
            i++;
        }
        int end = i;
        // we iterate through input queue and insert each process to the ready queue
        for (int j=0; j < end-start; j++) {
            input_t* input = dequeue(&inputs);
            process_t* p = input->process;
            free(input);
            ready_insert(ready_queue, p);
        }
        // now we run shortest job process by decrementing quantum each time
        if (running == NULL) {
            running = extract_max(ready_queue);
            if (running != NULL) {
                running->p_status = RUNNING;
                print_running(timer, running->name, running->time_left);
            }
            else {
                timer += fill_quantum;
                fill_quantum = quantum;
                num_quantum++;
            }
        }
        else {
            // job finishes within given quantum
            if (running->time_left <= fill_quantum) {
                timer += running->time_left;
                fill_quantum -= running->time_left;
                print_finished(timer, running->name, running->time_left);
                running->p_status = FINISHED;
                running->completed_time = timer;
                running = NULL;
            }
            else {
                running->time_left -= fill_quantum;
                timer += fill_quantum;
                fill_quantum = quantum;
                num_quantum++;
            }
        }
        condition = ready_queue->size > 0;
    }
    // free memory
    free(inputs);
    free(ready_queue);
    print_statistics(buffer, size);
}
