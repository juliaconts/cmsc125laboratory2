#include <stdio.h>
#include <string.h>
#include "scheduler.h"
#include "process.h"
#include "utils.h"
#include "gantt.h"

#define MAX_BLOCKS 10000
#define MAX_MESSAGES 1000

int schedule_stcf(SchedulerState *state)
{
    printf("Using STCF (Preemptive SJF)\n");

    Queue ready_queue;
    init_queue(&ready_queue);

    int completed = 0;
    int time = state->current_time;
    int context_switches = 0;

    Process *current = NULL;
    Process *prev_process = NULL;

    char prev_pid[16] = "";

    state->num_blocks = 0;
    init_gantt_chart(state->gantt_blocks, MAX_BLOCKS);

    char messages[MAX_MESSAGES][128];
    int num_messages = 0;

    while (completed < state->num_processes)
    {
        // 1. Add arrivals
        for (int i = 0; i < state->num_processes; i++)
        {
            if (state->processes[i].arrival_time == time)
            {
                enqueue(&ready_queue, &state->processes[i]);
            }
        }

        // 2. If current finished
        if (current != NULL && current->remaining_time == 0)
        {
            current->finish_time = time;
            current->turnaround_time = current->finish_time - current->arrival_time;
            current->waiting_time = current->turnaround_time - current->burst_time;
            current->response_time = current->start_time - current->arrival_time;

            completed++;
            current = NULL;
        }

        // 🚨 STOP immediately if all done (fixes extra [-])
        if (completed == state->num_processes)
            break;

        // 3. Put current back (preemption decision happens here)
        if (current != NULL)
        {
            enqueue(&ready_queue, current);
            current = NULL;
        }

        // 4. Pick shortest
        if (!is_empty(&ready_queue))
        {
            Process *next = dequeue_shortest(&ready_queue);

            // First execution
            if (next->start_time == -1)
                next->start_time = time;

            // Context switch + messages
            if (prev_process != NULL && strcmp(prev_process->pid, next->pid) != 0)
            {
                // Only mark preemption if previous process NOT finished
                if (prev_process->remaining_time > 0)
                {
                    snprintf(messages[num_messages++], 128,
                             "Process %s was preempted at t=%d",
                             prev_process->pid, time);
                }

                snprintf(messages[num_messages++], 128,
                         "Process %s resumed at t=%d",
                         next->pid, time);

                context_switches++;
            }

            prev_process = next;
            strcpy(prev_pid, next->pid);
            current = next;
        }

        // 5. Execute
        if (current != NULL)
        {
            current->remaining_time--;

            record_gantt(state->gantt_blocks,
                         &state->num_blocks,
                         MAX_BLOCKS,
                         current->pid,
                         time,
                         1);
        }
        else
        {
            record_gantt(state->gantt_blocks,
                         &state->num_blocks,
                         MAX_BLOCKS,
                         "-",
                         time,
                         1);
        }

        time++;
    }

    state->current_time = time;

    // ✅ Print Gantt chart FIRST
    print_gantt_chart(state->gantt_blocks, state->num_blocks);

    // ✅ Average response time
    double total_response = 0.0;
    for (int i = 0; i < state->num_processes; i++)
        total_response += state->processes[i].response_time;

    printf("\nTotal context switches: %d\n", context_switches);
    printf("Average response time: %.2f\n\n",
           total_response / state->num_processes);

    // ✅ Print messages AFTER
    for (int i = 0; i < num_messages; i++)
        printf("%s\n", messages[i]);

    return 0;
}