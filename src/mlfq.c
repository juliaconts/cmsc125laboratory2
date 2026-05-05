#include <stdio.h>
#include <string.h>
#include "scheduler.h"
#include "utils.h"
#include "gantt.h"
#include "mlfq.h"

int schedule_mlfq(SchedulerState *state, MLFQConfig *cfg)
{
    printf("\n=== MLFQ Configuration ===\n");
    for (int i = 0; i < cfg->num_queues; i++)
    {
        if (cfg->time_quantum[i] < 0)
            printf("Queue %d: FCFS (lowest priority)\n", i);
        else
            printf("Queue %d: q=%d, allotment=%d\n", i, cfg->time_quantum[i], cfg->allotment[i]);
    }
    printf("Boost period: %d\n\n", cfg->boost_period);

    Queue queues[MAX_QUEUES];
    for (int i = 0; i < cfg->num_queues; i++)
        init_queue(&queues[i]);

    // Initialize process metadata
    for (int i = 0; i < state->num_processes; i++)
    {
        state->processes[i].priority = 0;
        state->processes[i].time_in_queue = 0;
    }

    int time = state->current_time;
    int completed = 0;
    Process *current = NULL;
    int time_slice = 0;
    state->num_blocks = 0;

    // Analysis tracking arrays
    int final_queue[MAX_PROCESSES];
    int first_demotion_time[MAX_PROCESSES];
    for (int i = 0; i < state->num_processes; i++)
    {
        final_queue[i] = 0;
        first_demotion_time[i] = -1;
    }

    printf("=== Execution Trace ===\n");

    while (completed < state->num_processes)
    {
        // 1. Handle arrivals at the current tick
        for (int i = 0; i < state->num_processes; i++)
        {
            if (state->processes[i].arrival_time == time && state->processes[i].remaining_time > 0)
            {
                enqueue(&queues[0], &state->processes[i]);
                printf("t=%d: Process %s enters Q0\n", time, state->processes[i].pid);
            }
        }

        // 2. Priority Boost logic
        if (cfg->boost_period > 0 && time > 0 && time % cfg->boost_period == 0)
        {
            printf("t=%d: Priority boost\n", time);
            for (int i = 1; i < cfg->num_queues; i++)
            {
                while (!is_empty(&queues[i]))
                {
                    Process *p = dequeue(&queues[i]);
                    p->priority = 0;
                    p->time_in_queue = 0;
                    enqueue(&queues[0], p);
                }
            }
            if (current != NULL)
            {
                current->priority = 0;
                current->time_in_queue = 0;
                enqueue(&queues[0], current);
                current = NULL;
                time_slice = 0;
            }
        }

        // 3. Check for process completion (FIX: Prevents final idle tick)
        if (current != NULL && current->remaining_time == 0)
        {
            current->finish_time = time;
            current->turnaround_time = current->finish_time - current->arrival_time;
            current->waiting_time = current->turnaround_time - current->burst_time;
            current->response_time = current->start_time - current->arrival_time;

            printf("t=%d: Process %s completed\n", time, current->pid);
            completed++;
            current = NULL;
            time_slice = 0;

            if (completed == state->num_processes)
                break;
        }

        // 4. Select next process from highest priority non-empty queue
        if (current == NULL)
        {
            for (int i = 0; i < cfg->num_queues; i++)
            {
                if (!is_empty(&queues[i]))
                {
                    current = dequeue(&queues[i]);
                    time_slice = 0;
                    if (current->start_time == -1)
                        current->start_time = time;
                    printf("t=%d: Process %s selected from Q%d\n", time, current->pid, current->priority);
                    break;
                }
            }
        }

        // 5. Execute process for 1 tick
        if (current != NULL)
        {
            record_gantt(state->gantt_blocks, &state->num_blocks, MAX_BLOCKS, current->pid, time, 1);
            current->remaining_time--;
            current->time_in_queue++;
            time_slice++;

            // Check Allotment (Demotion)
            if (current->remaining_time > 0 && current->priority < cfg->num_queues - 1 &&
                cfg->allotment[current->priority] > 0 && current->time_in_queue >= cfg->allotment[current->priority])
            {

                current->priority++;
                current->time_in_queue = 0;
                printf("t=%d: Process %s -> Q%d (Allotment exhausted)\n", time + 1, current->pid, current->priority);

                // Update analysis metadata
                for (int i = 0; i < state->num_processes; i++)
                {
                    if (&state->processes[i] == current)
                    {
                        if (current->priority > final_queue[i])
                            final_queue[i] = current->priority;
                        if (first_demotion_time[i] == -1)
                            first_demotion_time[i] = time + 1;
                        break;
                    }
                }
                enqueue(&queues[current->priority], current);
                current = NULL;
                time_slice = 0;
            }
            // Check Quantum (Preemption)
            else if (current->remaining_time > 0 && cfg->time_quantum[current->priority] > 0 &&
                     time_slice >= cfg->time_quantum[current->priority])
            {
                printf("t=%d: Process %s -> Q%d (Quantum expired)\n", time + 1, current->pid, current->priority);
                enqueue(&queues[current->priority], current);
                current = NULL;
                time_slice = 0;
            }
        }
        else
        {
            record_gantt(state->gantt_blocks, &state->num_blocks, MAX_BLOCKS, "-", time, 1);
        }

        time++;
    }

    state->current_time = time;
    print_gantt_chart(state->gantt_blocks, state->num_blocks);

    // --- Performance Analysis Report ---
    printf("\n=== Analysis ===\n");

    // Interactive Job Behavior (Never demoted)
    int shortest_idx = -1;
    int interactive_count = 0;
    double resp_sum_interactive = 0.0;
    for (int i = 0; i < state->num_processes; i++)
    {
        if (first_demotion_time[i] == -1)
        {
            resp_sum_interactive += state->processes[i].response_time;
            interactive_count++;
            if (shortest_idx == -1 || state->processes[i].burst_time < state->processes[shortest_idx].burst_time)
                shortest_idx = i;
        }
    }

    if (interactive_count > 0)
    {
        printf("Interactive job behavior:\n");
        printf("  - Process %s stayed in Q0 (burst: %d)\n", state->processes[shortest_idx].pid, state->processes[shortest_idx].burst_time);
        printf("  - Avg response time for interactive jobs: %.1f\n", resp_sum_interactive / interactive_count);
    }

    // Long-running Job Behavior (Demoted at least once)
    int longest_idx = -1;
    for (int i = 0; i < state->num_processes; i++)
    {
        if (first_demotion_time[i] != -1)
        {
            if (longest_idx == -1 || final_queue[i] > final_queue[longest_idx] ||
                (final_queue[i] == final_queue[longest_idx] && state->processes[i].burst_time > state->processes[longest_idx].burst_time))
                longest_idx = i;
        }
    }

    if (longest_idx >= 0)
    {
        printf("\nLong-running job behavior:\n");
        printf("  - Process %s demoted to Q%d (first demotion at t=%d)\n",
               state->processes[longest_idx].pid, final_queue[longest_idx], first_demotion_time[longest_idx]);
        printf("  - Turnaround time: %d\n", state->processes[longest_idx].turnaround_time);
    }

    return 0;
}