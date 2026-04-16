#include <stdio.h>
#include <string.h>
#include "scheduler.h"
#include "process.h"
#include "utils.h"
#include "gantt.h"

int schedule_rr(SchedulerState *state, int quantum)
{
    printf("\nRunning RR Scheduler...");
    printf("\nUsing time quantum q=%d\n", quantum);

    Queue ready_queue;
    init_queue(&ready_queue); // starts with an empty FIFO queue

    int completed = 0;
    int time = state->current_time;
    int context_switches = 0;

    Process *current = NULL;
    int time_slice = 0;

    char prev_pid[16] = "";

    state->num_blocks = 0;
    init_gantt_chart(state->gantt_blocks, MAX_BLOCKS);

    // runs one tick at a time until every process has finished
    while (completed < state->num_processes)
    {
        // 1. Add newly arriving processes (checks every process to see if it arrives at this tick)
        for (int i = 0; i < state->num_processes; i++)
        {
            if (state->processes[i].arrival_time == time)
            {
                enqueue(&ready_queue, &state->processes[i]);
            }
        }

        // 2. If CPU idle, select the next process
        if (current == NULL && !is_empty(&ready_queue))
        {
            Process *next = dequeue(&ready_queue);

            // Count context switch ONLY if switching from another process
            if (prev_pid[0] != '\0' && strcmp(prev_pid, next->pid) != 0)
            {
                context_switches++;
            }

            current = next;
            time_slice = 0;

            // Record the very first run this process gets the CPU
            if (current->start_time == -1)
                current->start_time = time;

            strcpy(prev_pid, current->pid); // keeps track who is running for the next context-switch comparison
        }

        // 3. Execute
        if (current != NULL)
        {
            current->remaining_time--;
            time_slice++;

            record_gantt(state->gantt_blocks, &state->num_blocks, MAX_BLOCKS, current->pid, time, 1);

            // Case A: Process finished
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
            // Case B: Quantum expired → PREEMPT
            else if (time_slice == quantum)
            {
                enqueue(&ready_queue, current);
                current = NULL;
            }
        }
        else
        {
            // CPU idle
            record_gantt(state->gantt_blocks, &state->num_blocks, MAX_BLOCKS,
                         "-", time, 1);
        }

        time++;
    }

    state->current_time = time;

    // Print Gantt chart
    print_gantt_chart(state->gantt_blocks, state->num_blocks);

    // Compute average response time
    double total_response = 0;
    for (int i = 0; i < state->num_processes; i++)
        total_response += state->processes[i].response_time;

    double avg_response = total_response / state->num_processes;

    printf("\nTotal context switches: %d\n", context_switches);
    printf("Average response time: %.2f\n", avg_response);

    return 0;
}