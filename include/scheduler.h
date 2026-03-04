#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "process.h"

// Core simulation state [cite: 973-983]
typedef struct {
    Process *processes;    // Array of all processes
    int num_processes;     // Number of processes
    int current_time;      // Current simulation time
} SchedulerState;

// Discrete-event definitions [cite: 994-1005]
typedef enum {
    EVENT_ARRIVAL,
    EVENT_COMPLETION,
    EVENT_QUANTUM_EXPIRE,
    EVENT_PRIORITY_BOOST
} EventType;

typedef struct Event {
    int time;
    EventType type;
    Process *process;
    struct Event *next;
} Event;

// Function prototypes for the algorithms [cite: 984-986]
int schedule_fcfs(SchedulerState *state);
int schedule_sjf(SchedulerState *state);
int schedule_stcf(SchedulerState *state);
int schedule_rr(SchedulerState *state, int quantum);

#endif