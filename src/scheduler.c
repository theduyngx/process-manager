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


/* clear all processes in buffer */
void clear_buffer(process_t* buffer[], int size) {
    for (int i=0; i < size; i++)
        process_terminate(buffer[i]);
}

/* notify successful memory allocation and process getting to input queue */
void print_ready(uint32_t timer, const char* name, unsigned int base) {
    printf("%u,READY,process_name=%s,assigned_at=%u\n", timer, name, base);
}

/* print running process */
void print_running(uint32_t timer, const char* name, uint32_t time_left) {
    printf("%u,RUNNING,process_name=%s,remaining_time=%u\n", timer, name, time_left);
}

/* print finished process */
void print_finished(uint32_t timer, const char* name, int queue_size) {
    printf("%u,FINISHED,process_name=%s,proc_remaining=%d\n", timer, name, queue_size);
}

/* print scheduling statistics */
void print_statistics(process_t* buffer_arr[], int num_process, uint32_t makespan) {

    // get required data for statistics
    double total_turnaround = 0;
    double total_overhead = 0;
    double max_overhead = 0;
    for (int i=0; i < num_process; i++) {
        process_t* p = buffer_arr[i];
        uint32_t turnaround = p->completed_time - p->arrival;
        total_turnaround += turnaround;
        double overhead = (double) turnaround / p->service_time;
        total_overhead += overhead;
        if (overhead > max_overhead) max_overhead = overhead;
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

    // cleanup
    clear_buffer(buffer_arr, num_process);
}


/* Shortest job first scheduler */
void SJF_scheduler(queue_t* buffer, memory_t* mem, unsigned int quantum) {
    queue_t* input_queue = queue_init();
    heap_t* ready_queue = heap_init();
    uint32_t timer = 0;

    process_t* running = NULL;
    int size_condition = 1;
    while (buffer->size > 0 || running != NULL || size_condition) {

        // while all processes in buffer arrive within a quantum, we enqueue them to input queue
        int i = 0;
        int init_size = input_queue->size;
        while (buffer->size > 0 && buffer->node->process->arrival <= timer) {
            process_t* p = dequeue(buffer);
            enqueue(input_queue, p);
            i++;
        }

        // we iterate through input queue and insert each process to the ready queue
        for (int j=0; j < init_size + i; j++) {
            process_t* p = input_queue->node->process;

            // memory allocation - success means getting pushed to input queue
            // otherwise either wait, or process too expensive and must be killed
            unsigned int assigned_base;
            if (allocate_memory(mem, p, &assigned_base) == FAILURE)
                continue;
            print_ready(timer, p->name, assigned_base);
            dequeue(input_queue);
            heap_push(ready_queue, p);
        }

        // if no running process, then either get from ready queue or currently no process requesting
        if (running == NULL) {
            running = heap_pop(ready_queue);
            if (running != NULL) {
                running->status = RUNNING;
                print_running(timer, running->name, running->time_left);
            }
            else timer += quantum;
        }

        // otherwise, we run the current process
        else {
            timer += quantum;
            // job finishes within given quantum
            if (running->time_left <= quantum) {
                running->time_left = 0;
                if (deallocate_memory(mem, running) == FAILURE)
                    exit(1);
                print_finished(timer, running->name, ready_queue->size);
                running->status = FINISHED;
                running->completed_time = timer;
                running = NULL;
            }
            else running->time_left -= quantum;
        }
        size_condition = ready_queue->size > 0;
    }
    // free memory
    free_queue(input_queue);
    free_heap(ready_queue);
//    print_statistics(buffer, size, timer);
}


/* Round-robin */
void RR_scheduler(queue_t* buffer, memory_t* mem, unsigned int quantum) {
    queue_t* input_queue = queue_init();
    queue_t* ready_queue = queue_init();
    uint32_t timer = 0;

    process_t* running = NULL;
    int size_condition = 1;
    while (buffer->size > 0 || running != NULL || size_condition) {

        // while all processes in buffer arrive within a quantum, we enqueue them to input queue
        int init_size = input_queue->size;
        int i = 0;
        while (buffer->size > 0 && buffer->node->process->arrival <= timer) {
            process_t* p = dequeue(buffer);
            enqueue(input_queue, p);
            i++;
        }

        // we iterate through input queue and insert each process to the ready queue
        for (int j=0; j < init_size + i; j++) {
            process_t* p = input_queue->node->process;

            // memory allocation - success means getting pushed to input queue
            // otherwise either wait, or process too expensive and must be killed
            unsigned int assigned_base;
            if (allocate_memory(mem, p, &assigned_base) == FAILURE)
                continue;
            print_ready(timer, p->name, assigned_base);
            dequeue(input_queue);
            enqueue(ready_queue, p);
        }

        // round-robin
        if (ready_queue->size > 0 && running != NULL) {
            running->status = READY;
            enqueue(ready_queue, running);
            running = NULL;
        }
        // if there are processes in ready queue, or currently running process exists
        else if (ready_queue->size > 0 || running != NULL) {

            // if there is no process currently running
            if (running == NULL) {
                running = dequeue(ready_queue);
                running->status = RUNNING;
                print_running(timer, running->name, running->time_left);
            }
            int finished = running->time_left <= quantum;
            timer += quantum;

            // if, or otherwise, the process has finished its run
            if (finished) {
                running->time_left = 0;
                if (deallocate_memory(mem, running) == FAILURE)
                    exit(1);
                print_finished(timer, running->name, ready_queue->size);
                running->status = FINISHED;
                running->completed_time = timer;
                running = NULL;
            } else running->time_left -= quantum;
        }

        // otherwise - no process running nor any process in ready queue
        else timer += quantum;
        size_condition = ready_queue->size > 0;
    }
    // free memory
    free_queue(input_queue);
    free_queue(ready_queue);
//    print_statistics(buffer, size, timer);
}


/* ------------------------ OTHER SCHEDULING ALGORITHMS -------------------------- */


/* Shortest remaining time next scheduling - with memory allocation.
 */
__attribute__((unused))

void SRTN_scheduler(queue_t* buffer, memory_t* mem, unsigned int quantum) {
    queue_t* input_queue = queue_init();
    heap_t* ready_queue = heap_init();
    uint32_t timer = 0;

    process_t* running = NULL;
    int size_condition = 1;
    while (buffer->size > 0 || running != NULL || size_condition) {

        // while all processes in buffer arrive within a quantum, we enqueue them to input queue
        int init_size = input_queue->size;
        int i = 0;
        while (buffer->size > 0 && buffer->node->process->arrival <= timer) {
            process_t* p = dequeue(buffer);
            enqueue(input_queue, p);
            i++;
        }

        // we iterate through input queue and insert each process to the ready queue
        for (int j=0; j < init_size + i; j++) {
            process_t* p = input_queue->node->process;

            // memory allocation - success means getting pushed to input queue
            // otherwise either wait, or process too expensive and must be killed
            unsigned int assigned_base;
            if (allocate_memory(mem, p, &assigned_base) == FAILURE)
                continue;
            print_ready(timer, p->name, assigned_base);
            dequeue(input_queue);
            heap_push(ready_queue, p);
        }

        // round-robin
        if (ready_queue->size > 0 && running != NULL) {
            running->status = READY;
            heap_push(ready_queue, running);
            running = NULL;
        }
            // if there are processes in ready queue, or currently running process exists
        else if (ready_queue->size > 0 || running != NULL) {

            // if there is no process currently running
            if (running == NULL) {
                running = heap_pop(ready_queue);
                running->status = RUNNING;
                print_running(timer, running->name, running->time_left);
            }
            int finished = running->time_left <= quantum;
            timer += quantum;

            // if, or otherwise, the process has finished its run
            if (finished) {
                running->time_left = 0;
                if (deallocate_memory(mem, running) == FAILURE)
                    exit(1);
                print_finished(timer, running->name, ready_queue->size);
                running->status = FINISHED;
                running->completed_time = timer;
                running = NULL;
            } else running->time_left -= quantum;
        }

            // otherwise - no process running nor any process in ready queue
        else timer += quantum;
        size_condition = ready_queue->size > 0;
    }
    // free memory
    free_queue(input_queue);
    free_heap(ready_queue);
//    print_statistics(buffer, size, timer);
}
