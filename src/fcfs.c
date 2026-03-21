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
    Process *current = NULL; // pointer to the currently running process; NULL means CPU is idle

    state->num_blocks = 0; // reset Gantt chart

    while (completed < num_processes)
    {
        // 1. Selecting the Next Process (only picks a new process when CPU is free)
        if (current == NULL)
        {
            int earliest_index = -1;

            for (int i = 0; i < num_processes; i++) // checks a process that has already arrived
            {
                if (processes[i].arrival_time <= current_time && processes[i].remaining_time > 0) // if the arrived process has work to do
                {
                    if (earliest_index == -1 || processes[i].arrival_time < processes[earliest_index].arrival_time) // if arrived earlier than the current best candidate
                        earliest_index = i;
                }
            }
            if (earliest_index != -1) // selects a process that is ready
            {
                current = &processes[earliest_index];
                if (current->start_time == -1)
                    current->start_time = current_time;
            }
        }

        // 2. Handles CPU idle (no process is ready yet, there is a gap between the start and the next process's arrival)
        if (current == NULL)
        {
            record_gantt(state->gantt_blocks, &state->num_blocks, "-", current_time, 1);
            current_time++;
            continue;
        }

        // 3. Execute process to completion
        for (int t = 0; t < current->remaining_time; t++)
        {
            record_gantt(state->gantt_blocks, &state->num_blocks, current->pid, current_time, 1);
            current_time++;
        }

        // 4. Record completion metrics
        current->finish_time = current_time;
        current->turnaround_time = current->finish_time - current->arrival_time;
        current->waiting_time = current->turnaround_time - current->burst_time;
        current->response_time = current->start_time - current->arrival_time;

        current->remaining_time = 0;
        completed++;    // records the processes that are done
        current = NULL; // resets the CPU for the next process
    }

    print_gantt_chart(state->gantt_blocks, state->num_blocks);
    return 0;
}