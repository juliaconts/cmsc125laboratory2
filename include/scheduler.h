#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "process.h"
#include "gantt.h"

// Core simulation state
typedef struct
{
    Process *processes; // Array of all processes
    int num_processes;  // Number of processes
    int current_time;   // Current simulation time

    GanttBlock gantt_blocks[MAX_BLOCKS];
    int num_blocks;
} SchedulerState;

// Discrete-event definitions
typedef enum
{
    EVENT_ARRIVAL,
    EVENT_COMPLETION,
    EVENT_QUANTUM_EXPIRE,
    EVENT_PRIORITY_BOOST
} EventType;

typedef struct Event
{
    int time;
    EventType type;
    Process *process;
    struct Event *next;
} Event;

// Function prototypes for the algorithms
int schedule_fcfs(SchedulerState *state);
int schedule_sjf(SchedulerState *state);
int schedule_stcf(SchedulerState *state);
int schedule_rr(SchedulerState *state, int quantum);

int schedule_sjf(SchedulerState *state);
int schedule_stcf(SchedulerState *state);

#endif