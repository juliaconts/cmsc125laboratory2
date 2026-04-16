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
        {
            printf("Queue %d: FCFS (lowest priority)\n", i);
        }
        else
        {
            printf("Queue %d: q=%d, allotment=%d\n",
                   i, cfg->time_quantum[i], cfg->allotment[i]);
        }
    }
    printf("Boost period: %d\n\n", cfg->boost_period);
    fflush(stdout);

    // Initialize queues per priority level
    Queue queues[MAX_QUEUES];
    for (int i = 0; i < cfg->num_queues; i++)
    {
        init_queue(&queues[i]);
    }

    // Initialize processes
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
    init_gantt_chart(state->gantt_blocks, MAX_BLOCKS);

    // Analysis tracking
    // final_queue[i]          : highest queue number the process ever reached
    // first_demotion_time[i]  : sim time of first demotion (-1 = never demoted)
    int final_queue[MAX_PROCESSES];
    int first_demotion_time[MAX_PROCESSES];
    for (int i = 0; i < state->num_processes; i++)
    {
        final_queue[i] = 0;
        first_demotion_time[i] = -1;
    }

    printf("=== Execution Trace ===\n");
    fflush(stdout);

    while (completed < state->num_processes)
    {
        // 1. Handle arrivals
        for (int i = 0; i < state->num_processes; i++)
        {
            // Guard remaining_time > 0 so a finished process that happens
            // to share an arrival_time value is never double-enqueued.
            if (state->processes[i].arrival_time == time &&
                state->processes[i].remaining_time > 0)
            {
                enqueue(&queues[0], &state->processes[i]);
                printf("t=%d: Process %s enters Q0\n",
                       time, state->processes[i].pid);
                fflush(stdout);
            }
        }

        // 2. Priority Boost
        if (cfg->boost_period > 0 && time > 0 && time % cfg->boost_period == 0)
        {
            printf("t=%d: Priority boost\n", time);
            fflush(stdout);

            // Drain Q1..Qn-1 into Q0  (leave Q0 alone)
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

            // Preempt the currently running process
            if (current != NULL)
            {
                current->priority = 0;
                current->time_in_queue = 0;
                enqueue(&queues[0], current);
                current = NULL;
                time_slice = 0;
            }
            // Step 3 selects the next process fairly.
        }

        // 3. Pick next process if CPU idle
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
                    printf("t=%d: Process %s selected from Q%d\n",
                           time, current->pid, current->priority);
                    fflush(stdout);
                    break;
                }
            }
        }

        // 4. Execute current process
        if (current != NULL)
        {
            current->remaining_time--;
            current->time_in_queue++;
            time_slice++;

            record_gantt(state->gantt_blocks, &state->num_blocks, MAX_BLOCKS,
                         current->pid, time, 1);

            // Process finished
            if (current->remaining_time == 0)
            {
                current->finish_time = time + 1;
                current->turnaround_time = current->finish_time - current->arrival_time;
                current->waiting_time = current->turnaround_time - current->burst_time;
                current->response_time = current->start_time - current->arrival_time;

                printf("t=%d: Process %s completed\n", time + 1, current->pid);
                fflush(stdout);
                completed++;
                current = NULL;
                time_slice = 0;
            }

            // Allotment exhausted → demote
            else if (current->priority < cfg->num_queues - 1 &&
                     cfg->allotment[current->priority] > 0 &&
                     current->time_in_queue >= cfg->allotment[current->priority])
            {
                current->priority++;
                current->time_in_queue = 0;
                printf("t=%d: Process %s -> Q%d\n",
                       time + 1, current->pid, current->priority);
                fflush(stdout);

                // Track highest queue reached and time of first demotion
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

            // Quantum expiration (within the same priority level)
            else if (cfg->time_quantum[current->priority] > 0 &&
                     time_slice >= cfg->time_quantum[current->priority])
            {
                // Allotment accumulates across quanta — do NOT reset
                // time_in_queue here.
                enqueue(&queues[current->priority], current);
                current = NULL;
                time_slice = 0;
            }

            // Periodic flush so output appears even during long FCFS
            // stretches where none of the above branches fire.
            else if (time % 50 == 0)
            {
                fflush(stdout);
            }
        }
        else
        {
            // CPU idle
            record_gantt(state->gantt_blocks, &state->num_blocks, MAX_BLOCKS, "-", time, 1);
        }

        time++;
    }

    state->current_time = time;

    printf("\n");
    fflush(stdout);
    print_gantt_chart(state->gantt_blocks, state->num_blocks);
    fflush(stdout);

    // Analysis Report
    printf("\n=== Analysis ===\n");

    // Interactive jobs: never demoted (stayed in Q0)
    // Sorted by burst_time ascending so the shortest job is reported first.
    printf("Interactive job (short burst) behavior:\n");
    double resp_sum_interactive = 0.0;
    int interactive_count = 0;
    int shortest_idx = -1;

    for (int i = 0; i < state->num_processes; i++)
    {
        if (first_demotion_time[i] == -1) /* never demoted = stayed in Q0 */
        {
            resp_sum_interactive += state->processes[i].response_time;
            interactive_count++;
            if (shortest_idx == -1 ||
                state->processes[i].burst_time <
                    state->processes[shortest_idx].burst_time)
            {
                shortest_idx = i;
            }
        }
    }

    if (interactive_count > 0)
    {
        Process *sp = &state->processes[shortest_idx];
        printf("  - Process %s stayed in Q0 (completed in %d time units)\n",
               sp->pid, sp->burst_time);
        printf("  - Average response time: %.1f\n",
               resp_sum_interactive / interactive_count);
    }
    else
    {
        printf("  - All processes were eventually demoted (no pure interactive jobs)\n");
    }

    // Long-running jobs: demoted at least once
    // Report the process with the highest final queue level;
    // break ties by largest burst_time (most CPU-intensive).
    printf("\nLong-running job behavior:\n");
    int longest_idx = -1;

    for (int i = 0; i < state->num_processes; i++)
    {
        if (first_demotion_time[i] != -1)
        {
            if (longest_idx == -1 ||
                final_queue[i] > final_queue[longest_idx] ||
                (final_queue[i] == final_queue[longest_idx] &&
                 state->processes[i].burst_time >
                     state->processes[longest_idx].burst_time))
            {
                longest_idx = i;
            }
        }
    }

    if (longest_idx >= 0)
    {
        Process *lp = &state->processes[longest_idx];
        printf("  - Process %s demoted to Q%d after %d time units\n",
               lp->pid, final_queue[longest_idx],
               first_demotion_time[longest_idx]);
        printf("  - Turnaround time: %d (fair for its burst time of %d)\n",
               lp->turnaround_time, lp->burst_time);
    }
    else
    {
        printf("  - No long-running jobs were demoted\n");
    }

    // Overall verdict
    // Responsiveness check: avg response of interactive jobs vs long-running
    int any_interactive = (interactive_count > 0);
    int any_longrunning = (longest_idx >= 0);

    if (any_interactive && any_longrunning)
    {
        double resp_long = 0.0;
        int long_count = 0;
        for (int i = 0; i < state->num_processes; i++)
        {
            if (first_demotion_time[i] != -1)
            {
                resp_long += state->processes[i].response_time;
                long_count++;
            }
        }
        double avg_resp_interactive = resp_sum_interactive / interactive_count;
        double avg_resp_long = resp_long / long_count;

        if (avg_resp_interactive <= avg_resp_long)
            printf("\nYour MLFQ successfully balanced responsiveness and fairness.\n");
        else
            printf("\nYour MLFQ completed all jobs but long-running processes had "
                   "better response times than interactive ones — consider "
                   "tuning your allotment or boost period.\n");
    }
    else
    {
        printf("\nYour MLFQ completed all jobs successfully.\n");
    }

    fflush(stdout);
    return 0;
}