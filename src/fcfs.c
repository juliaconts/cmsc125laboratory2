#include <stdio.h>
#include <string.h>
#include "scheduler.h"
#include "gantt.h"

int schedule_fcfs(SchedulerState *state)
{
    int completed = 0;
    Process *processes = state->processes;
    int num_processes = state->num_processes;
    int current_time = 0;
    Process *current = NULL;

    state->num_blocks = 0; // reset Gantt chart

    while (completed < num_processes)
    {
        // Pick next process (earliest arrival)
        if (current == NULL)
        {
            int earliest_index = -1;
            for (int i = 0; i < num_processes; i++)
            {
                if (processes[i].arrival_time <= current_time && processes[i].remaining_time > 0)
                {
                    if (earliest_index == -1 || processes[i].arrival_time < processes[earliest_index].arrival_time)
                        earliest_index = i;
                }
            }
            if (earliest_index != -1)
            {
                current = &processes[earliest_index];
                if (current->start_time == -1)
                    current->start_time = current_time;
            }
        }

        // If no process is ready, idle
        if (current == NULL)
        {
            record_gantt(state->gantt_blocks, &state->num_blocks, "-", current_time, 1);
            current_time++;
            continue;
        }

        // Execute process to completion
        for (int t = 0; t < current->remaining_time; t++)
        {
            record_gantt(state->gantt_blocks, &state->num_blocks, current->pid, current_time, 1);
            current_time++;
        }

        current->finish_time = current_time;
        current->turnaround_time = current->finish_time - current->arrival_time;
        current->waiting_time = current->turnaround_time - current->burst_time;
        current->response_time = current->start_time - current->arrival_time;

        current->remaining_time = 0;
        completed++;
        current = NULL;
    }

    print_gantt_chart(state->gantt_blocks, state->num_blocks);
    return 0;
}