#include <stdio.h>
#include "scheduler.h"
#include "process.h"

int schedule_fcfs(SchedulerState *state)
{
    int completed = 0;
    Process *current = NULL;

    while (completed < state->num_processes)
    {
        // Select next process (FCFS = earliest arrival)
        if (current == NULL)
        {
            int earliest_index = -1;

            for (int i = 0; i < state->num_processes; i++)
            {
                Process *p = &state->processes[i];

                if (p->arrival_time <= state->current_time && p->remaining_time > 0)
                {
                    if (earliest_index == -1 || p->arrival_time < state->processes[earliest_index].arrival_time)
                    {
                        earliest_index = i;
                    }
                }
            }

            if (earliest_index != -1)
            {
                current = &state->processes[earliest_index];

                // First execution -> response time
                if (current->start_time == -1)
                {
                    current->start_time = state->current_time;
                }
            }
        }

        if (current != NULL)
        {
            current->remaining_time--;

            state->gantt_chart[state->current_time] = current->pid[0];

            if (current->remaining_time == 0)
            {
                current->finish_time = state->current_time + 1;

                current->turnaround_time = current->finish_time - current->arrival_time;

                current->waiting_time = current->turnaround_time - current->burst_time;

                current->response_time = current->start_time - current->arrival_time;

                completed++;
                current = NULL;
            }
            else
            {
                state->gantt_chart[state->current_time] = '-';
            }

            state->current_time++;
        }
    }
    return 0;
}