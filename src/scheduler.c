#include <stdio.h>
#include <stdlib.h>
#include "scheduler.h"
#include "utils.h"

void simulate_scheduler(SchedulerState *state) {
    Queue ready_queue;
    init_queue(&ready_queue);
    
    // Process *current_process = NULL; // Will be used when algorithms are hooked up
    
    printf("Starting simulation engine...\n");

    // The discrete time loop [cite: 1084-1105]
    for (int t = 0; !all_complete(state->processes, state->num_processes); t++) {
        
        // Handle process arrivals at current time 't' 
        for (int i = 0; i < state->num_processes; i++) {
            if (state->processes[i].arrival_time == t) {
                enqueue(&ready_queue, &state->processes[i]);
                
                // Track start time for response time metric 
                if (state->processes[i].start_time == -1) {
                    state->processes[i].start_time = t; 
                }
            }
        }

        // TODO: Algorithm selection and execution logic will go here
        
        // Safety break to prevent infinite loops during early testing
        if (t > 1000) {
            printf("Simulation timed out. Exiting loop.\n");
            break; 
        }
    }
    
    printf("Simulation complete.\n");
}