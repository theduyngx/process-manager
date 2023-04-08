#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "process_real.h"

int write_process(process_t* p, uint32_t timer) {
    char* args[] = {"./process", p->name, NULL};
    execv("./process", args);

    // convert the value to big endian byte ordering
    char* big_endian = (char*) &timer;
    int fd[2];
    pid_t child_pid;
    char read_buffer[80];

    // fork, error handling
    if ((child_pid = fork()) == -1) {
        fprintf(stderr, "Fork failed\n");
        exit(1);
    }

    // pipping
    pipe(fd);
    if (child_pid == 0) {
        // Child process closes up input side of pipe
        close(fd[0]);

        // Send "string" through the output side of pipe
        write(fd[1], big_endian, (strlen(big_endian) + 1));
        exit(0);
    }
    else {
        // Parent process closes up output side of pipe
        close(fd[1]);

        // Read in a string from the pipe
        read(fd[0], read_buffer, sizeof(read_buffer));
        printf("Received string: %s", read_buffer);
    }

    return 0;
}