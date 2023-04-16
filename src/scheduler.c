/*
 * Author  : The Duy Nguyen - 1100548
 * File    : scheduler.c
 * Purpose : Functions related to scheduling processes.
 */

#include <stdio.h>
#include <stdlib.h>
#include "queue.h"
#include "scheduler.h"

#include "process_real.h"


/**
 * First-come, First-serve scheduler, scheduling processes using the queue data structure.
 * @param buffer    all input processes
 * @param mem       the memory
 * @param quantum   quantum length (s)
 */
__attribute__((unused))
void FCFS_scheduler(queue_t* buffer, memory_t* mem, unsigned int quantum) {
    nonpreemptive_scheduler(buffer, mem, quantum, QUEUE);
}


/**
 * Shortest job first scheduler, scheduling processes using the heap data structure.
 * @param buffer    all input processes
 * @param mem       the memory
 * @param quantum   quantum length (s)
 */
void SJF_scheduler(queue_t* buffer, memory_t* mem, unsigned int quantum) {
    nonpreemptive_scheduler(buffer, mem, quantum, HEAP);
}


/**
 * Round-robin scheduler - preemptive. The scheduler uses queue data structure to manage processes.
 * @param buffer    all processes
 * @param mem       the memory
 * @param quantum   quantum length
 */
void RR_scheduler(queue_t* buffer, memory_t* mem, unsigned int quantum) {
    preemptive_scheduler(buffer, mem, quantum, QUEUE);
}


/**
 * Shortest remaining time next scheduler - preemptive. The scheduler uses heap to manage processes.
 * It always executes the job with least service time left, and with preemption.
 * @param buffer    all processes
 * @param mem       the memory
 * @param quantum   quantum length
 */
__attribute__((unused))
void SRTN_scheduler(queue_t* buffer, memory_t* mem, unsigned int quantum) {
    preemptive_scheduler(buffer, mem, quantum, HEAP);
}


/* -------------------------------- SCHEDULER TEMPLATES -------------------------------- */

/**
 * Template for nonpreemptive scheduling algorithms. The priority of the nonpreemptive algorithm
 * will be determined by the ready queue type parameter - the data type used for the ready queue
 * to extract and manage its priority.
 * @param buffer    process buffer
 * @param mem       the memory
 * @param quantum   the quantum (amount of time per cycle)
 * @param rq_type   data structure type of ready queue
 */
void nonpreemptive_scheduler(queue_t* buffer, memory_t* mem, unsigned int quantum, enum structure rq_type) {
    // error handling
    if (buffer == NULL || buffer->node == NULL) {
        fprintf(stderr, "ERROR - nonpreemptive_scheduler: null buffer\n");
        exit(1);
    }
    if (mem == NULL) {
        fprintf(stderr, "ERROR - nonpreemptive_scheduler: null memory\n");
        exit(1);
    }

    // queues and timer
    queue_t* input_queue = queue_init();
    ready_queue_t* ready_queue = ready_queue_init(rq_type);
    uint32_t timer = 0;

    /// metadata of finished processes
    process_t* finished[buffer->size];
    int index = 0;

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
            continue_process(running, timer);
            timer += quantum;

            // job finishes within given quantum
            if (running->time_left <= quantum)
                finish_process(&running, mem, finished, &index, timer,
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


/**
 * Template for preemptive scheduling algorithms. The priority of the preemptive algorithm
 * will be determined by the ready queue type parameter - the data type used for the ready
 * queue to extract and manage its priority.
 * @param buffer    process buffer
 * @param mem       the memory
 * @param quantum   the quantum (amount of time per cycle)
 * @param rq_type   data structure type of ready queue
 */
void preemptive_scheduler(queue_t* buffer, memory_t* mem, unsigned int quantum, enum structure rq_type) {
    // error handling
    if (buffer == NULL || buffer->node == NULL) {
        fprintf(stderr, "ERROR - preemptive_scheduler: null buffer\n");
        exit(1);
    }
    if (mem == NULL) {
        fprintf(stderr, "ERROR - preemptive_scheduler: null memory\n");
        exit(1);
    }

    // queues and timer
    queue_t* input_queue = queue_init();
    ready_queue_t* ready_queue = ready_queue_init(rq_type);
    uint32_t timer = 0;

    /// metadata of finished processes
    process_t* finished[buffer->size];
    int index = 0;

    process_t* running = NULL;
    int size_condition = 1;
    while (buffer->size > 0 || running != NULL || size_condition) {

        // update input and ready queues
        update_queues(buffer, mem, input_queue, ready_queue, timer);

        // preemption
        if (ready_queue->size > 0 && running != NULL) {
            // suspending process
//            suspend_process(running, timer);

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
            // continue process
            continue_process(running, timer);
            int completed = running->time_left <= quantum;
            timer += quantum;

            // if, or otherwise, the process has completed its execution
            if (completed)
                finish_process(&running, mem, finished, &index, timer,
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


/* -------------------------------- HELPER FUNCTIONS -------------------------------- */

/**
 * Update the input and ready queues; called after each quantum.
 * @param buffer        the process buffer
 * @param mem           the memory for processes to be allocated
 * @param input_queue   the input queue
 * @param ready_queue   the ready queue
 * @param timer         the timer
 */
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
        if (mem->requirement != INF)
            print_ready(timer, p, assigned_base);
        dequeue(input_queue);
        insert(ready_queue, p);

        // create a real process
        create_process(p, timer);
    }
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
void finish_process(process_t** running, memory_t* mem, process_t** finished, int *index,
                    uint32_t timer, int proc_remaining)
{
    (*running)->time_left = 0;
    if (deallocate_memory(mem, *running) == FAILURE) {
        exit(3);
    }
    // terminate real process
    (*running)->status = FINISHED;
    (*running)->completed_time = timer;
    finished[(*index)++] = *running;
    terminate_process(*running, timer);

    print_finished(timer, *running, proc_remaining);
    *running = NULL;

}


/**
 * Clear all processes in buffer
 * @param buffer  array of all processes
 * @param size    size of array
 */
void clear_buffer(process_t* buffer[], int size) {
    for (int i=0; i < size; i++)
        process_free(buffer[i]);
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
 * Print scheduling statistics
 * @param all_processes the array of all processes; usually obtaining the statistics occurs
 *                      after processes have been finished so this is the finished array
 * @param num_process   total number of processes
 * @param makespan      total amount of time (s) taken to complete all processes in buffer
 */
void print_statistics(process_t* all_processes[], int num_process, uint32_t makespan) {

    // get required data for statistics
    double total_turnaround = 0;
    double total_overhead = 0;
    double max_overhead = 0;
    for (int i=0; i < num_process; i++) {
        process_t* p = all_processes[i];
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
