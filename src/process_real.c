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
    int READ_END = 0;
    int WRITE_END = 1;
    char* PATH = "./process";

    // convert the value to big endian byte ordering
    char endian_arr[NUM_ENDIAN_BYTES];
    big_endian(endian_arr, timer);
    uint32_t converted = htonl(timer);

    // forking and pipping
    int parent_fd[2];
    int child_fd[2];
    pipe(parent_fd);
    pipe(child_fd);
    pid_t curr_id = fork();

    // fork error
    if (curr_id == -1) {
        fprintf(stderr, "ERROR - create_process: fork failed\n");
        exit(1);
    }

    // process program - child
    else if (curr_id == 0) {
        dup2(parent_fd[READ_END], STDIN_FILENO);
        dup2(child_fd[WRITE_END], STDOUT_FILENO);
        char* args[] = {PATH, p->name, NULL};
        execv(PATH, args);
    }

    // allocate program - parent
    else {
        write(parent_fd[WRITE_END], &converted, sizeof(converted));
        char read_buffer[1];
        read(child_fd[READ_END], read_buffer, sizeof(read_buffer));
        long unsigned int lsb = NUM_ENDIAN_BYTES - 1;
        if (read_buffer[0] != endian_arr[lsb]) {
            fprintf(stderr,
                    "ERROR - create_process: process %s differs in read and written byte\n",
                    p->name);
            printf("%x %x \n", (&converted)[lsb], read_buffer[0]);
            exit(2);
        }
        p->pid = curr_id;

        // temporarily terminate process
        kill(p->pid, SIGTERM);

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
    char* endian_arr = (char*) &timer;
    int fd[2];
    char read_buffer[80];
    pipe(fd);
    write(fd[1], endian_arr, (strlen(endian_arr) + 1));
    read(fd[0], read_buffer, sizeof(read_buffer));

    kill(p->pid, SIGSTOP);

    int status;
    if (waitpid(p->pid, &status, WUNTRACED) > 0) {
        if (!WIFEXITED(status) || WEXITSTATUS(status) == 127)
            exit(1);
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
    char* endian_arr = (char*) &timer;
    int fd[2];
    char read_buffer[80];
    pipe(fd);
    write(fd[1], endian_arr, (strlen(endian_arr) + 1));
    read(fd[0], read_buffer, sizeof(read_buffer));

    kill(p->pid, SIGCONT);

    int status;
    if (waitpid(p->pid, &status, WCONTINUED) > 0) {
        if (!WIFEXITED(status) || WEXITSTATUS(status) == 127)
            exit(1);
    }
    return 0;
}


/**
 * Terminate a process.
 * @param p     pseudo-process, or metadata of the real process
 * @return      0 if successful, and otherwise if not
 */
int terminate_process(process_t* p) {
    kill(p->pid, SIGTERM);
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
