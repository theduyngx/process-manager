/*
 * Author  : The Duy Nguyen - 1100548
 * File    : main.c
 * Purpose : The main program, receiving input from user.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

#include "queue.h"
#include "scheduler.h"
#include "mem_alloc.h"

#define INLINE_COUNT 4
#define MAX_NAME_LEN 8


/**
 * Printing to standard error - since no flag can be empty when running the program.
 * @param flag  the flag (single character precede by a dash)
 */
void print_empty_flag(char* flag) {
    fprintf(stderr, "Flag %s must be included\n", flag);
}


/**
 * The main entry to program.
 * @return 0 if succeeds and otherwise if fails
 */
int main(int argc, char* argv[]) {
    char* f_flag = NULL;
    char* s_flag = NULL;
    char* m_flag = NULL;
    char* q_flag = NULL;
    int c;

    // check input flags
    while ((c = getopt(argc, argv, "f:s:m:q:")) != -1) {
        switch (c) {
            case 'f':
                f_flag = optarg;
                break;
            case 's':
                s_flag = optarg;
                break;
            case 'm':
                m_flag = optarg;
                break;
            case 'q':
                q_flag = optarg;
                break;
            case '?':
                fprintf(stderr, "Error in option -%c. Aborting...\n", optopt);
            default:
                exit(1);
        }
    }
    // check if any flag is left empty
    if (f_flag == NULL) {
        print_empty_flag("-f");
        exit(1);
    }
    if (s_flag == NULL) {
        print_empty_flag("-s");
        exit(1);
    }
    if (m_flag == NULL) {
        print_empty_flag("-m");
        exit(1);
    }
    if (q_flag == NULL) {
        print_empty_flag("-q");
        exit(1);
    }

    // open file, check for file flag correctness
    if (! strstr(f_flag, ".txt")) {
        fprintf(stderr, "Invalid file extension for flag -f: %s\n", f_flag);
        exit(1);
    }
    FILE *input = fopen(f_flag, "r");
    if (input == NULL) {
        fprintf(stderr, "Invalid argument for flag -f: file %s doesn't exist\n", f_flag);
        exit(1);
    }

    // read the file by line
    queue_t* buffer = queue_init();
    int request_time, service_time, size;
    char name[MAX_NAME_LEN];
    while (fscanf(input, "%d %s %d %d", &request_time, name, &service_time, &size)
           == INLINE_COUNT)
    {
        process_t* p = process_init(name, request_time, service_time, size);
        enqueue(buffer, p);
    }
    fclose(input);


    // check for memory flag value correctness
    memory_t* mem;
    if      (strcmp(m_flag, "infinite") == 0) mem = memory_inf_init();
    else if (strcmp(m_flag, "best-fit") == 0) mem = memory_init(FINITE_CAPACITY);
    else {
        fprintf(stderr, "Invalid argument for flag -m: %s\n", m_flag);
        exit(2);
    }

    // check quantum flag value correctness
    int quantum = atoi(q_flag);   // NOLINT(cert-err34-c)
    if (quantum <= 0) {
        fprintf(stderr, "Invalid argument for flag -q: %s\n", q_flag);
        free_memory(mem);
        exit(2);
    }

    // check for scheduling algorithm flag
    if      (strcmp(s_flag, "SJF" ) == 0) SJF_scheduler (buffer, mem, quantum);
    else if (strcmp(s_flag, "FCFS") == 0) FCFS_scheduler(buffer, mem, quantum);
    else if (strcmp(s_flag, "RR"  ) == 0) RR_scheduler  (buffer, mem, quantum);
    else if (strcmp(s_flag, "SRTN") == 0) SRTN_scheduler(buffer, mem, quantum);
    else {
        fprintf(stderr, "Invalid argument for flag -s: %s\n", s_flag);
        free_memory(mem);
        exit(2);
    }

    return 0;
}
