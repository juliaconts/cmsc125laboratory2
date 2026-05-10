#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "process.h"
#include "gantt.h"
#include "mlfq.h"

// Core simulation state
typedef struct
{
    Process *processes; // Array of all processes
    int num_processes;  // Number of processes
    int current_time;   // Current simulation time

    GanttBlock gantt_blocks[MAX_BLOCKS];
    int num_blocks;
} SchedulerState;

// Function prototypes for the algorithms
int schedule_fcfs(SchedulerState *state);
int schedule_sjf(SchedulerState *state);
int schedule_stcf(SchedulerState *state);
int schedule_rr(SchedulerState *state, int quantum);
int schedule_mlfq(SchedulerState *state, MLFQConfig *cfg);

#endif