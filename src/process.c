#include <string.h>
#include "process.h"

// initializes a process structure with default values.

void init_process(Process *p, const char *pid, int arrival, int burst)
{
    strncpy(p->pid, pid, 15);
    p->pid[15] = '\0';

    p->arrival_time = arrival;
    p->burst_time = burst;
    p->remaining_time = burst;

    // initialize metrics to -1 or 0 to indicate they havent been recorded yet
    p->start_time = -1;
    p->finish_time = 0;
    p->turnaround_time = 0;
    p->waiting_time = 0;
    p->response_time = 0;

    // MLFQ specific fields
    p->priority = 0;
    p->time_in_queue = 0;
}