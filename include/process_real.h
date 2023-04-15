
#ifndef PROJECT1_CLION_PROCESS_REAL_H
#define PROJECT1_CLION_PROCESS_REAL_H


#include "pseudo_process.h"

int create_process(process_t* p, uint32_t timer);
int suspend_process(process_t* p, uint32_t timer);
int continue_process(process_t* p, uint32_t timer);

void big_endian(char order[4], uint32_t value);


#endif //PROJECT1_CLION_PROCESS_REAL_H
