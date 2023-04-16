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
    int ENDIAN_BYTE_SIZE = 4;
    char endian_arr[ENDIAN_BYTE_SIZE];
    big_endian(endian_arr, timer);

    // forking and pipping
    int fd[2];
    char read_buffer[ENDIAN_BYTE_SIZE];
    pid_t curr_id = fork();
    pipe(fd);

    // fork error
    if (curr_id == -1) {
        fprintf(stderr, "ERROR - create_process: fork failed\n");
        exit(1);
    }

    // child process
    else if (curr_id == 0) {
        p->pid = getpid();
        dup2(fd[WRITE_END], STDIN_FILENO);
        ////
        close(fd[READ_END]);
        close(fd[WRITE_END]);
        ////
        char* args[] = {PATH, p->name, "-v", NULL};
        execv(PATH, args);
        write(fd[WRITE_END], endian_arr, ENDIAN_BYTE_SIZE);
    }

    // parent process
    else {
        dup2(fd[READ_END], STDOUT_FILENO);
        ////
        close(fd[READ_END]);
        close(fd[WRITE_END]);
        ////
        read(fd[READ_END], read_buffer, ENDIAN_BYTE_SIZE);
        long unsigned int lsb = ENDIAN_BYTE_SIZE - 1;
        if (read_buffer[lsb] != endian_arr[lsb]) {
            fprintf(stderr,
                    "ERROR - create_process: process %s differs in read and written byte\n",
                    p->name);
            printf("%x %x \n", endian_arr[lsb], read_buffer[lsb]);
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
void big_endian(char order[4], uint32_t value) {
    uint32_t converted = htonl(value);
    *(uint32_t*) order = converted;
}
