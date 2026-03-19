#ifndef GANTT_H
#define GANTT_H

#include "process.h"
#include <stdio.h>
#include <string.h>

#define MAX_BLOCKS 10000

typedef struct
{
    char pid[16];
    int start_time;
    int duration;
} GanttBlock;

// Initialize Gantt blocks array
void init_gantt_chart(GanttBlock *blocks, int max_blocks);

// Record a new block (or extend last if same PID)
void record_gantt(GanttBlock *blocks, int *num_blocks, const char *pid, int start_time, int duration);

// Print Gantt chart with optional scaling
void print_gantt_chart(GanttBlock *blocks, int num_blocks);

#endif