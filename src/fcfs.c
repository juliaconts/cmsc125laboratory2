#include <stdio.h>
#include "scheduler.h"
#include "process.h"

int schedue_fcfs(SchedulerState *state)
{
    int completed = 0;
    Process *current = NULL;

    while (completed < state->num_processes)
    {
        if (current == NULL)
        {
            for (int i = 0; i < state->num_processes; i++)
            {
                Process *p = &state->processes[i];

                if (p->arrival_time <= state->current_time && p->remaining_time > 0)
                {
                    current = p;

                    if (!p->started)
                    {
                        p->start_time = state->current_time;
                        p->started = 1;
                    }
                    break;
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
                completed++;
                current = NULL;
            }
        }
        else
        {
            state->gantt_chart[state->current_time] = '-';
        }

        state->current_time++;
    }
    return 0;
}