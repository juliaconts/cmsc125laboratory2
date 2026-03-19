#include <stdio.h>
#include <string.h>
#include "scheduler.h"
#include "process.h"
#include "utils.h" // Queue
#include "gantt.h"

#define MAX_BLOCKS 10000
#define MAX_MESSAGES 1000

int schedule_rr(SchedulerState *state, int quantum)
{
    printf("Using time quantum q=%d\n", quantum);

    Queue ready_queue;
    init_queue(&ready_queue);

    int completed = 0;
    int time = state->current_time;
    int context_switches = 0;

    Process *current = NULL;
    int time_slice = 0;
    char prev_pid[16] = "";

    state->num_blocks = 0;
    init_gantt_chart(state->gantt_blocks, MAX_BLOCKS);

    // Collect messages to print later
    char messages[MAX_MESSAGES][128];
    int num_messages = 0;

    while (completed < state->num_processes)
    {
        // Add arriving processes to ready queue
        for (int i = 0; i < state->num_processes; i++)
        {
            Process *p = &state->processes[i];
            if (p->arrival_time == time)
                enqueue(&ready_queue, p);
        }

        // Pick next process if CPU is idle
        if (current == NULL && !is_empty(&ready_queue))
        {
            current = dequeue(&ready_queue);
            time_slice = 0;

            // First execution → response time
            if (current->start_time == -1)
                current->start_time = time;

            // Collect preemption/resume messages
            if (strcmp(prev_pid, current->pid) != 0 && prev_pid[0] != '\0')
            {
                snprintf(messages[num_messages++],
                         128,
                         "Process %s was preempted at t=%d (remaining: %d)",
                         prev_pid, time, current->remaining_time);

                snprintf(messages[num_messages++],
                         128,
                         "Process %s resumed at t=%d",
                         current->pid, time);

                context_switches++;
            }

            strcpy(prev_pid, current->pid);
        }

        // Execute current process
        if (current != NULL)
        {
            current->remaining_time--;
            time_slice++;

            // Record in Gantt chart
            record_gantt(state->gantt_blocks, &state->num_blocks,
                         current->pid, time, 1);

            // Process finished
            if (current->remaining_time == 0)
            {
                current->finish_time = time + 1;
                current->turnaround_time = current->finish_time - current->arrival_time;
                current->waiting_time = current->turnaround_time - current->burst_time;
                current->response_time = current->start_time - current->arrival_time;

                completed++;
                current = NULL;
                context_switches++;
            }
            // Quantum expired → preempt
            else if (time_slice == quantum)
            {
                enqueue(&ready_queue, current);
                current = NULL;
                context_switches++;
            }
        }
        else
        {
            // CPU idle
            record_gantt(state->gantt_blocks, &state->num_blocks, "-", time, 1);
        }

        time++;
    }

    state->current_time = time;

    // Print Gantt chart
    print_gantt_chart(state->gantt_blocks, state->num_blocks);

    // Compute average response time
    double total_response = 0.0;
    for (int i = 0; i < state->num_processes; i++)
        total_response += state->processes[i].response_time;

    double avg_response = total_response / state->num_processes;

    printf("\nTotal context switches: %d\n", context_switches);
    printf("Average response time: %.2f\n\n", avg_response);

    // Print preemption/resume messages collected during execution
    for (int i = 0; i < num_messages; i++)
        printf("%s\n", messages[i]);

    return 0;
}