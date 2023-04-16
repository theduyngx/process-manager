/*
 * Author  : The Duy Nguyen - 1100548
 * File    : process_real.c
 * Purpose : Functions related to real processes. It is not responsible for creating, continuing,
 *           suspending, and terminating real processes.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <arpa/inet.h>

#include "process_real.h"


/**
 * Create a real process. It reads the timer in big endian byte ordering to the process.
 * @param p     pseudo-process, or metadata of the real process
 * @param timer the current time
 * @return      0 if successful, and otherwise if not
 */
int create_process(process_t* p, uint32_t timer) {
    char* PATH = "./process";

    // convert the value to big endian byte ordering
    char endian_arr[NUM_ENDIAN_BYTES];
    big_endian(endian_arr, timer);
    uint32_t converted = htonl(timer);

    // forking and pipping
    pipe(p->parent_fd);
    pipe(p->child_fd);
    p->pid = fork();

    // fork error
    if (p->pid == -1) {
        fprintf(stderr, "ERROR - create_process: fork failed\n");
        exit(1);
    }

    // process program - child
    else if (p->pid == 0) {
        dup2(p->parent_fd[READ_END], STDIN_FILENO);
        dup2(p->child_fd[WRITE_END], STDOUT_FILENO);
        char* args[] = {PATH, p->name, NULL};
        execv(PATH, args);
    }

    // allocate program - parent
    else {
        write(p->parent_fd[WRITE_END], &converted, sizeof(converted));
        char read_buffer[1];
        read(p->child_fd[READ_END], read_buffer, sizeof(read_buffer));
        long unsigned int lsb = NUM_ENDIAN_BYTES - 1;
        if (read_buffer[0] != endian_arr[lsb]) {
            fprintf(stderr,
                    "ERROR - create_process: process %s differs in read and written byte\n",
                    p->name);
            fprintf(stderr, "written    : 0x%x\n", endian_arr[lsb]);
            fprintf(stderr, "read buffer: 0x%x\n", read_buffer[0]);
            exit(2);
        }
    }
    return 0;
}


/**
 * Continue a process.
 * @param p     pseudo-process, or metadata of the real process
 * @param timer the current time
 * @return      0 if successful, and otherwise if not
 */
int continue_process(process_t* p, uint32_t timer) {

    // convert the value to big endian byte ordering
    char endian_arr[NUM_ENDIAN_BYTES];
    big_endian(endian_arr, timer);
    uint32_t converted = htonl(timer);

    if (p->pid == 0) {
        dup2(p->parent_fd[READ_END], STDIN_FILENO);
        dup2(p->child_fd[WRITE_END], STDOUT_FILENO);
        printf("IN 0\n");
    }
    else {
        write(p->parent_fd[WRITE_END], &converted, sizeof(converted));

        // send signal to process
        kill(p->pid, SIGCONT);

        char read_buffer[1];
        read(p->child_fd[READ_END], read_buffer, sizeof(read_buffer));
        long unsigned int lsb = NUM_ENDIAN_BYTES - 1;
        if (read_buffer[0] != endian_arr[lsb]) {
            fprintf(stderr,
                    "ERROR - continue_process: process %s differs in read and written byte\n",
                    p->name);
            fprintf(stderr, "written    : 0x%x\n", endian_arr[lsb]);
            fprintf(stderr, "read buffer: 0x%x\n", read_buffer[0]);
            exit(2);
        }
    }
    return 0;
}


/**
 * Suspend process.
 * @param p     pseudo-process, or metadata of the real process
 * @param timer the current time
 * @return      0 if successful, and otherwise if not
 */
int suspend_process(process_t* p, uint32_t timer) {

    // convert the value to big endian byte ordering
    char endian_arr[NUM_ENDIAN_BYTES];
    big_endian(endian_arr, timer);
    uint32_t converted = htonl(timer);

    if (p->pid == 0) {
        dup2(p->parent_fd[READ_END], STDIN_FILENO);
        dup2(p->child_fd[WRITE_END], STDOUT_FILENO);
        printf("IN 0\n");
    }
    else {
        write(p->parent_fd[WRITE_END], &converted, sizeof(converted));

        // send signal to process
        kill(p->pid, SIGTSTP);
        int status;
        waitpid(p->pid, &status, WUNTRACED);
        if (WIFEXITED(status)) {

            char read_buffer[1];
            read(p->child_fd[READ_END], read_buffer, sizeof(read_buffer));
            long unsigned int lsb = NUM_ENDIAN_BYTES - 1;
            if (read_buffer[0] != endian_arr[lsb]) {
                fprintf(stderr,
                        "ERROR - suspend_process: process %s differs in read and written byte\n",
                        p->name);
                fprintf(stderr, "written    : 0x%x\n", endian_arr[lsb]);
                fprintf(stderr, "read buffer: 0x%x\n", read_buffer[0]);
                exit(2);
            }
        }
    }
    return 0;
}


/**
 * Terminate a process.
 * @param p     pseudo-process, or metadata of the real process
 * @return      0 if successful, and otherwise if not
 */
int terminate_process(process_t* p, uint32_t timer) {

    // convert the value to big endian byte ordering
    char endian_arr[NUM_ENDIAN_BYTES];
    big_endian(endian_arr, timer);
    uint32_t converted = htonl(timer);

    if (p->pid == 0) {
        dup2(p->parent_fd[READ_END], STDIN_FILENO);
        dup2(p->child_fd[WRITE_END], STDOUT_FILENO);
    }
    else {
        write(p->parent_fd[WRITE_END], &converted, sizeof(converted));

        // send signal to process
        kill(p->pid, SIGTERM);

        int NUM_SHA_BYTES = 64;
        char sha[NUM_SHA_BYTES+1];
        sha[NUM_SHA_BYTES] = '\0';
        read(p->child_fd[READ_END], sha, NUM_SHA_BYTES);
//        printf("%s\n", sha);
//        long unsigned int lsb = NUM_ENDIAN_BYTES - 1;
//        if (read_buffer[0] != endian_arr[lsb]) {
//            fprintf(stderr,
//                    "ERROR - terminate_process: process %s differs in read and written byte\n",
//                    p->name);
//            fprintf(stderr, "written    : 0x%x\n", endian_arr[lsb]);
//            fprintf(stderr, "read buffer: 0x%x\n", read_buffer[0]);
//            exit(2);
//        }
    }
    return 0;
}


/**
 * Convert an unsigned, 32-bit integer to its big endian byte ordering.
 * @param order big endian byte ordering
 * @param value the integer
 */
void big_endian(char order[NUM_ENDIAN_BYTES], uint32_t value) {
    uint32_t converted = htonl(value);
    *(uint32_t*) order = converted;
}
