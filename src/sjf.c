#include <stdio.h>
#include <stdlib.h>
#include "scheduler.h"
#include "utils.h"

int schedule_sjf(SchedulerState *state) {
    Queue ready_queue;
    init_queue(&ready_queue);
    Process *current = NULL;

    printf("Starting SJF Simulation...\n");

    for (int t = 0; !all_complete(state->processes, state->num_processes); t++) {
        // 1. Handle Arrivals
        for (int i = 0; i < state->num_processes; i++) {
            if (state->processes[i].arrival_time == t) {
                enqueue(&ready_queue, &state->processes[i]);
            }
        }

        // 2. Handle Completion
        if (current != NULL && current->remaining_time == 0) {
            current->finish_time = t;
            current = NULL; // CPU is now free
        }

        // 3. Schedule next process (Non-preemptive: only schedule if CPU is free)
        if (current == NULL && !is_empty(&ready_queue)) {
            current = dequeue_shortest(&ready_queue);
            
            // Record start time if this is the first time running
            if (current->start_time == -1) {
                current->start_time = t;
            }
        }

        // 4. Execute current process
        if (current != NULL) {
            current->remaining_time--;
        }
    }
    
    printf("SJF Simulation Complete.\n");
    return 0;
}