/*
 * Author  : The Duy Nguyen - 1100548
 * File    : scheduler.c
 * Purpose : Functions related to scheduling processes.
 */

#include <stdio.h>
#include <stdlib.h>
#include "queue.h"
#include "scheduler.h"
#include "ready_queue.h"

/**
 * Clear all processes in buffer
 * @param buffer  array of all processes
 * @param size    size of array
 */
void clear_buffer(process_t* buffer[], int size) {
    for (int i=0; i < size; i++)
        process_terminate(buffer[i]);
}

/**
 * Notify successful memory allocation and process getting to input queue.
 * @param timer the current time
 * @param p     the process in question
 * @param base  assigned memory base
 */
void print_ready(uint32_t timer, process_t* p, unsigned int base) {
    printf("%u,READY,process_name=%s,assigned_at=%u\n", timer, p->name, base);
}

/**
 * Print running process.
 * @param timer  the timer
 * @param p      the process
 */
void print_running(uint32_t timer, process_t* p) {
    printf("%u,RUNNING,process_name=%s,remaining_time=%u\n", timer, p->name, p->time_left);
}

/**
 * Print finished process, and number of remaining process in input and ready queue.
 * @param timer           the timer
 * @param p               the finished process
 * @param proc_remaining  number of remaining process to be executed
 */
void print_finished(uint32_t timer, process_t* p, int proc_remaining) {
    printf("%u,FINISHED,process_name=%s,proc_remaining=%d\n", timer, p->name, proc_remaining);
}

/**
 * Finish a process - implying also deallocating its memory, updating the finished array and whatnot.
 * @param running           running process now finished
 * @param mem               the memory
 * @param finished          finished processes array
 * @param index             current index of finished array
 * @param timer             the timer
 * @param proc_remaining    number of processes remaining
 */
void finish_process(process_t* running, memory_t* mem, process_t** finished, int *index,
                    uint32_t timer, int proc_remaining)
{
    running->time_left = 0;
    if (deallocate_memory(mem, running) == FAILURE)
        exit(1);
    print_finished(timer, running, proc_remaining);
    running->status = FINISHED;
    running->completed_time = timer;
    finished[(*index)++] = running;
    running = NULL;
}


void update_queues(queue_t* buffer, memory_t* mem, queue_t* input_queue, ready_queue_t* ready_queue,
                   uint32_t timer)
{
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
        print_ready(timer, p, assigned_base);
        dequeue(input_queue);
        insert(ready_queue, p);
    }
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
}


/* Shortest job first scheduler */
void SJF_scheduler(queue_t* buffer, memory_t* mem, unsigned int quantum) {
    queue_t* input_queue = queue_init();
    ready_queue_t* ready_queue = ready_queue_init(HEAP);
    uint32_t timer = 0;

    ///
    process_t* finished[buffer->size];
    int index = 0;
    ///

    process_t* running = NULL;
    int size_condition = 1;
    while (buffer->size > 0 || running != NULL || size_condition) {

        // update input and ready queues
        update_queues(buffer, mem, input_queue, ready_queue, timer);

        // if no running process, then either get from ready queue or currently no process requesting
        if (running == NULL) {
            running = extract(ready_queue);
            if (running != NULL) {
                running->status = RUNNING;
                print_running(timer, running);
            }
            else timer += quantum;
        }

        // otherwise, we run the current process
        else {
            timer += quantum;
            // job finishes within given quantum
            if (running->time_left <= quantum)
                finish_process(running, mem, finished, &index, timer,
                               ready_queue->size + input_queue->size);
            else running->time_left -= quantum;
        }
        size_condition = ready_queue->size > 0;
    }
    // print statistics
    print_statistics(finished, index, timer);

    // free memory
    free_queue(input_queue);
    free_ready_queue(ready_queue);
    clear_buffer(finished, index);
}


/* Round-robin */
void RR_scheduler(queue_t* buffer, memory_t* mem, unsigned int quantum) {
    queue_t* input_queue = queue_init();
    ready_queue_t* ready_queue = ready_queue_init(QUEUE);
    uint32_t timer = 0;

    ///
    process_t* finished[buffer->size];
    int index = 0;
    ///

    process_t* running = NULL;
    int size_condition = 1;
    while (buffer->size > 0 || running != NULL || size_condition) {

        // update input and ready queues
        update_queues(buffer, mem, input_queue, ready_queue, timer);

        // round-robin
        if (ready_queue->size > 0 && running != NULL) {
            running->status = READY;
            insert(ready_queue, running);
            running = NULL;
        }
        // if there are processes in ready queue, or currently running process exists
        else if (ready_queue->size > 0 || running != NULL) {

            // if there is no process currently running
            if (running == NULL) {
                running = extract(ready_queue);
                running->status = RUNNING;
                print_running(timer, running);
            }
            int completed = running->time_left <= quantum;
            timer += quantum;

            // if, or otherwise, the process has completed its execution
            if (completed)
                finish_process(running, mem, finished, &index, timer,
                               ready_queue->size + input_queue->size);
            else running->time_left -= quantum;
        }

        // otherwise - no process running nor any process in ready queue
        else timer += quantum;
        size_condition = ready_queue->size > 0;
    }
    // print statistics
    print_statistics(finished, index, timer);

    // free memory
    free_queue(input_queue);
    free_ready_queue(ready_queue);
    clear_buffer(finished, index);
}


/* ------------------------ OTHER SCHEDULING ALGORITHMS -------------------------- */


/* Shortest remaining time next scheduling - with memory allocation.
 */
__attribute__((unused))

void SRTN_scheduler(queue_t* buffer, memory_t* mem, unsigned int quantum) {
    queue_t* input_queue = queue_init();
    ready_queue_t* ready_queue = ready_queue_init(HEAP);
    uint32_t timer = 0;

    ///
    process_t* finished[buffer->size];
    int index = 0;
    ///

    process_t* running = NULL;
    int size_condition = 1;
    while (buffer->size > 0 || running != NULL || size_condition) {

        // update input and ready queues
        update_queues(buffer, mem, input_queue, ready_queue, timer);

        // round-robin
        if (ready_queue->size > 0 && running != NULL) {
            running->status = READY;
            insert(ready_queue, running);
            running = NULL;
        }
        // if there are processes in ready queue, or currently running process exists
        else if (ready_queue->size > 0 || running != NULL) {

            // if there is no process currently running
            if (running == NULL) {
                running = extract(ready_queue);
                running->status = RUNNING;
                print_running(timer, running);
            }
            int completed = running->time_left <= quantum;
            timer += quantum;

            // if, or otherwise, the process has completed its execution
            if (completed)
                finish_process(running, mem, finished, &index, timer,
                               ready_queue->size + input_queue->size);
            else running->time_left -= quantum;
        }

        // otherwise - no process running nor any process in ready queue
        else timer += quantum;
        size_condition = ready_queue->size > 0;
    }
    // print statistics
    print_statistics(finished, index, timer);

    // free memory
    free_queue(input_queue);
    free_ready_queue(ready_queue);
    clear_buffer(finished, index);
}
