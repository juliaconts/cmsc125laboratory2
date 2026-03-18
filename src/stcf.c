#include <stdio.h>
#include <stdlib.h>
#include "scheduler.h"
#include "utils.h"

int schedule_stcf(SchedulerState *state) {
    Queue ready_queue;
    init_queue(&ready_queue);
    Process *current = NULL;

    printf("Starting STCF Simulation...\n");

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
            current = NULL; 
        }

        // 3. Preemption Logic: Put the current process back in line!
        if (current != NULL) {
            enqueue(&ready_queue, current);
            current = NULL;
        }

        // 4. Schedule next process (Pick the shortest available every single tick)
        if (!is_empty(&ready_queue)) {
            current = dequeue_shortest(&ready_queue);
            
            if (current->start_time == -1) {
                current->start_time = t;
            }
        }

        // 5. Execute current process
        if (current != NULL) {
            current->remaining_time--;
        }
    }

    printf("STCF Simulation Complete.\n");
    return 0;
}