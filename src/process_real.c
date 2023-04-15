#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <arpa/inet.h>

#include "process_real.h"



void big_endian(char order[4], uint32_t value) {
    uint32_t converted = htonl(value);
    *(uint32_t*) order = converted;
}

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
        dup2(fd[WRITE_END], STDIN_FILENO);
        char* args[] = {PATH, p->name, NULL};
        execv(PATH, args);
        write(fd[WRITE_END], endian_arr, ENDIAN_BYTE_SIZE);
        exit(0);
    }

    // parent process
    dup2(fd[READ_END], STDOUT_FILENO);
    int status;
    if (waitpid(curr_id, &status, 0) > 0) {
        if (! WIFEXITED(status) || WEXITSTATUS(status) == 127)
            exit(1);
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


int suspend_process(process_t* p, uint32_t timer) {
    char* endian_arr = (char*) &timer;
    int fd[2];
    char read_buffer[80];
    pipe(fd);
    write(fd[1], endian_arr, (strlen(endian_arr) + 1));
    read(fd[0], read_buffer, sizeof(read_buffer));

    kill(p->pid, SIGSTOP);
    int w_status;
    pid_t w = waitpid(p->pid, &w_status, WUNTRACED);
    return 0;
}


int continue_process(process_t* p, uint32_t timer) {
    char* endian_arr = (char*) &timer;
    int fd[2];
    char read_buffer[80];
    pipe(fd);
    write(fd[1], endian_arr, (strlen(endian_arr) + 1));
    read(fd[0], read_buffer, sizeof(read_buffer));

    kill(p->pid, SIGCONT);
    int w_status;
    pid_t w = waitpid(p->pid, &w_status, WUNTRACED);
    return 0;
}


int process_terminate() {
    return 0;
}
