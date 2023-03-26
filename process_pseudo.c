#include <stdlib.h>

enum status {
    READY,
    RUNNING,
    FINISHED
};

struct process {
    char *name;
    int requested_time;
    int time_left;
    int memory_requirement;
    enum status p_status;
};
typedef struct process process_t;

void process_init(process_t *p, char *name, int request_time, int est_time, int memory) {
    p = (process_t *) malloc(sizeof(process_t));
    p->name = name;
    p->requested_time = request_time;
    p->time_left = est_time;
    p->memory_requirement = memory;
    p->p_status = READY;
}

int process_terminate(process_t *p) {
    if (p->p_status != FINISHED) return 1;
    free(p);
}
