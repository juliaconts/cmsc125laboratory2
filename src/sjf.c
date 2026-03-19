#include <stdio.h>
#include <stdlib.h>
#include "scheduler.h"
#include "utils.h"
#include "gantt.h"

int schedule_sjf(SchedulerState *state)
{
    Queue ready_queue;
    init_queue(&ready_queue);
    Process *current = NULL;

    printf("Starting SJF Simulation...\n");

    state->num_blocks = 0;
    init_gantt_chart(state->gantt_blocks, MAX_BLOCKS);

    int completed = 0;
    int time = 0;

    while (completed < state->num_processes)
    {
        // 1. Handle Arrivals
        for (int i = 0; i < state->num_processes; i++)
        {
            if (state->processes[i].arrival_time == time)
            {
                enqueue(&ready_queue, &state->processes[i]);
            }
        }

        // 2. Handle Completion
        if (current != NULL && current->remaining_time == 0)
        {
            current->finish_time = time;
            current->turnaround_time = current->finish_time - current->arrival_time;
            current->waiting_time = current->turnaround_time - current->burst_time;
            current->response_time = current->start_time - current->arrival_time;
            completed++;
            current = NULL; // CPU is now free
        }

        // 3. Schedule next process (Non-preemptive: only schedule if CPU is free)
        if (current == NULL && !is_empty(&ready_queue))
        {
            current = dequeue_shortest(&ready_queue);

            // Record start time if this is the first time running
            if (current->start_time == -1)
            {
                current->start_time = time;
            }
        }

        // 4. Execute current process
        if (current != NULL)
        {
            record_gantt(state->gantt_blocks, &state->num_blocks, current->pid, time, 1);
            current->remaining_time--;
        }
        else
        {
            record_gantt(state->gantt_blocks, &state->num_blocks, "-", time, 1);
        }

        time++;
    }
    print_gantt_chart(state->gantt_blocks, state->num_blocks);

    printf("\nSJF Simulation Complete.\n");
    return 0;
}