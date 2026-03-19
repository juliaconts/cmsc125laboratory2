#include <stdio.h>
#include "scheduler.h"
#include "process.h"
#include "utils.h" // for Queue

int schedule_rr(SchedulerState *state, int quantum)
{
    Queue ready_queue;
    init_queue(&ready_queue);

    int completed = 0;
    int time = state->current_time;

    Process *current = NULL;
    int time_slice = 0;

    while (completed < state->num_processes)
    {
        // 🔹 Add arriving processes to queue
        for (int i = 0; i < state->num_processes; i++)
        {
            Process *p = &state->processes[i];

            if (p->arrival_time == time)
            {
                enqueue(&ready_queue, p);
            }
        }

        // 🔹 If no current process, get one
        if (current == NULL && !is_empty(&ready_queue))
        {
            current = dequeue(&ready_queue);
            time_slice = 0;

            // First time execution → response time
            if (current->start_time == -1)
            {
                current->start_time = time;
            }
        }

        // 🔹 Execute current process
        if (current != NULL)
        {
            current->remaining_time--;
            time_slice++;

            state->gantt_chart[time] = current->pid[0];

            // ✅ If process finished
            if (current->remaining_time == 0)
            {
                current->finish_time = time + 1;

                current->turnaround_time =
                    current->finish_time - current->arrival_time;

                current->waiting_time =
                    current->turnaround_time - current->burst_time;

                current->response_time =
                    current->start_time - current->arrival_time;

                completed++;
                current = NULL;
            }
            // ✅ If quantum expired
            else if (time_slice == quantum)
            {
                enqueue(&ready_queue, current);
                current = NULL;
            }
        }
        else
        {
            // CPU idle
            state->gantt_chart[time] = '-';
        }

        time++;
    }

    state->current_time = time;
    return 0;
}