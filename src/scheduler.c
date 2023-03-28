/*
 * Author  : The Duy Nguyen - 1100548
 * File    : scheduler.c
 * Purpose : Functions related to scheduling processes.
 */

#include <stdio.h>
#include <stdlib.h>
#include "queue.h"
#include "heap.h"
#include "scheduler.h"


/* print running process */
void print_running(uint32_t timer, const char* name, uint32_t time_left) {
    printf("%u,RUNNING,process_name=%s,remaining_time=%u\n", timer, name, time_left);
}

/* print finished process */
void print_finished(uint32_t timer, const char* name, int queue_size) {
    printf("%u,FINISHED,process_name=%s,proc_remaining=%d\n", timer, name, queue_size);
}

/* print scheduling statistics */
void print_statistics(process_t* buffer[], int num_process, uint32_t makespan) {
    double total_turnaround = 0;
    double total_overhead = 0;
    double max_overhead = 0;
    for (int i=0; i < num_process; i++) {
        process_t* p = buffer[i];
        uint32_t turnaround = p->completed_time - p->arrival;
        total_turnaround += turnaround;
        double overhead = (double) turnaround / p->service_time;
        total_overhead += overhead;
        if (overhead > max_overhead) max_overhead = overhead;

        // FOR NOW: free processes here
        process_terminate(p);

    }
    // turnaround
    double turnaround_double = total_turnaround / num_process;
    uint32_t turnaround_floor = (int) total_turnaround / num_process;
    uint32_t turnaround = turnaround_floor + (turnaround_floor < turnaround_double);
    printf("Turnaround time %u\n", turnaround);

    // overhead
    double average_overhead = total_overhead / num_process;
    printf("Time overhead %.2f %.2f\n", max_overhead, average_overhead);

    // makespan
    printf("Makespan %u\n", makespan);
}


/* Shortest job first scheduler */
void SJF_scheduler(process_t* buffer[], int size, unsigned int quantum) {
    queue_t* input_queue = queue_init();
    heap_t* ready_queue = heap_init();
    uint32_t timer = 0;
    int i = 0;
    int num_quantum = 0;

    process_t* running = NULL;
    int condition = 1;
    while (i < size || condition || running != NULL) {

        // while all processes in buffer arrive within a quantum, we enqueue them to input queue
        int start = i;
        while (i < size && buffer[i]->arrival <= timer) {
            process_t* p = buffer[i];
            enqueue(input_queue, p);
            i++;
        }
        // we iterate through input queue and insert each process to the ready queue
        for (int j=0; j < i-start; j++) {
            // NOTE: for part 3 - memory allocation, just add a condition here where memory must be
            // successfully allocated in order to be enqueued to the ready_queue
            qnode_t* input = dequeue(input_queue);
            process_t* p = input->process;
            free(input);
            heap_insert(ready_queue, p);
        }
        // now we run shortest job process by decrementing quantum each time
        if (running == NULL) {
            running = extract_min(ready_queue);
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
                print_finished(timer, running->name, ready_queue->size);
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
    free(input_queue);
    free(ready_queue);
    print_statistics(buffer, size, timer);
}


/* Round-robin */
void RR_scheduler(process_t* buffer[], int size, unsigned int quantum) {
    queue_t* input_queue = queue_init();
    queue_t* ready_queue = queue_init();
    uint32_t timer = 0;
    int i = 0;
    int num_quantum = 0;

    process_t* running = NULL;
    int condition = 1;
    while (i < size || condition || running != NULL) {

        // while all processes in buffer arrive within a quantum, we enqueue them to input queue
        int start = i;
        while (i < size && buffer[i]->arrival <= timer) {
            process_t* p = buffer[i];
            enqueue(input_queue, p);
            i++;
        }
        // we iterate through input queue and insert each process to the ready queue
        for (int j=0; j < i-start; j++) {
            // NOTE: for part 3 - memory allocation, just add a condition here where memory must be
            // successfully allocated in order to be enqueued to the ready_queue
            qnode_t* input = dequeue(input_queue);
            process_t* p = input->process;
            free(input);
            enqueue(ready_queue, p);
        }

        // round-robin
        if (ready_queue->size > 0 && running != NULL) {
            running->p_status = READY;
            enqueue(ready_queue, running);
            running = NULL;
        }
        // if there are processes in ready queue, or currently running process exists
        else if (ready_queue->size > 0 || running != NULL) {

            // if there is no process currently running
            if (running == NULL) {
                running = dequeue(ready_queue)->process;
                running->p_status = RUNNING;
                print_running(timer, running->name, running->time_left);
            }
            int finished = running->time_left <= quantum;
            timer += quantum;
            num_quantum++;

            // if, or otherwise, the process has finished its run
            if (finished) {
                print_finished(timer, running->name, ready_queue->size);
                running->p_status = FINISHED;
                running->completed_time = timer;
                running = NULL;
            } else running->time_left -= quantum;
        }
        // otherwise - no process running nor any process in ready queue
        else {
            timer += quantum;
            num_quantum++;
        }
        condition = ready_queue->size > 0;
    }
    // free memory
    free(input_queue);
    free(ready_queue);
    print_statistics(buffer, size, timer);
}


/* Shortest job first quantum optimized scheduler */
void SJF_scheduler_optimized(process_t* buffer[], int size, unsigned int quantum) {
    queue_t* input_queue = queue_init();
    heap_t* ready_queue = heap_init();
    uint32_t timer = 0;
    int i = 0;
    int num_quantum = 0;
    // fill quantum is the hole in the quantum yet to be filled
    uint32_t fill_quantum = quantum;

    process_t* running = NULL;
    int condition = 1;
    while (i < size || condition || running != NULL) {

        // while all processes in buffer arrive within a quantum, we enqueue them to input queue
        // NOTE: modulo operation implies we only check this after a quantum has fully elapsed
        int start = i;
        while (i < size && timer % quantum == 0 && buffer[i]->arrival <= timer) {
            process_t* p = buffer[i];
            enqueue(input_queue, p);
            i++;
        }
        // we iterate through input queue and insert each process to the ready queue
        for (int j=0; j < i-start; j++) {
            qnode_t* input = dequeue(input_queue);
            process_t* p = input->process;
            free(input);
            heap_insert(ready_queue, p);
        }
        // now we run shortest job process by decrementing quantum each time
        if (running == NULL) {
            running = extract_min(ready_queue);
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
                print_finished(timer, running->name, ready_queue->size);
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
    free(input_queue);
    free(ready_queue);
    print_statistics(buffer, size, timer);
}
