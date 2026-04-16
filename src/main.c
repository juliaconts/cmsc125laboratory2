#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "process.h"
#include "scheduler.h"
#include "metrics.h"
#include "gantt.h"
#include "mlfq.h"

// Helper function to parse the workload file
Process *parse_workload_file(const char *filename, int *num_processes)
{
    FILE *file = fopen(filename, "r");
    if (!file)
    {
        perror("Error opening workload file");
        return NULL;
    }

    // Start with space for 10 processes, expand if needed
    int capacity = 10;
    Process *processes = malloc(capacity * sizeof(Process));
    if (!processes)
    {
        perror("Memory allocation failed");
        fclose(file);
        return NULL;
    }

    *num_processes = 0;
    char line[256];

    // Read file line by line
    while (fgets(line, sizeof(line), file))
    {
        // Skip comments starting with '#' and empty lines
        if (line[0] == '#' || line[0] == '\n' || line[0] == '\r')
            continue;

        char pid[16];
        int arrival, burst;

        // Format: PID ArrivalTime BurstTime
        if (sscanf(line, "%15s %d %d", pid, &arrival, &burst) == 3)
        {
            // Expand array dynamically if we hit the limit
            if (*num_processes >= capacity)
            {
                capacity *= 2;
                Process *temp_ptr = realloc(processes, capacity * sizeof(Process));
                if (temp_ptr == NULL)
                {
                    perror("Critical Error: Memory reallocation failed");
                    free(processes);
                    fclose(file);
                    return NULL;
                }
                processes = temp_ptr;
            }

            init_process(&processes[*num_processes], pid, arrival, burst);
            (*num_processes)++;

            // // Initialize process fields securely
            // strncpy(processes[*num_processes].pid, pid, 15);
            // processes[*num_processes].pid[15] = '\0'; // Ensure null-termination
            // processes[*num_processes].arrival_time = arrival;
            // processes[*num_processes].burst_time = burst;

            // // Set defaults for the simulation tracking fields
            // processes[*num_processes].remaining_time = burst;
            // processes[*num_processes].start_time = -1; // -1 means hasn't started yet
            // processes[*num_processes].finish_time = 0;
            // processes[*num_processes].turnaround_time = 0;
            // processes[*num_processes].waiting_time = 0;
            // processes[*num_processes].response_time = 0;
            // processes[*num_processes].priority = 0;
            // processes[*num_processes].time_in_queue = 0;

            // (*num_processes)++;
        }
    }

    fclose(file);
    return processes;
}

int load_mlfq_config(const char *filename, MLFQConfig *cfg)
{
    FILE *file = fopen(filename, "r");
    if (!file)
    {
        perror("Error opening MLFQ config file");
        return -1;
    }

    cfg->num_queues = 0;
    cfg->boost_period = 0;
    char line[256];

    while (fgets(line, sizeof(line), file))
    {
        if (line[0] == '#' || line[0] == '\n' || line[0] == '\r')
        {
            continue;
        }

        if (strncmp(line, "BOOST_PERIOD", 12) == 0)
        {
            sscanf(line, "BOOST_PERIOD %d", &cfg->boost_period);
        }
        else
        {
            char qid[10];
            int quantum, allot;

            if (sscanf(line, "%s %d %d", qid, &quantum, &allot) == 3)
            {
                int i = cfg->num_queues;

                cfg->time_quantum[i] = quantum;
                cfg->allotment[i] = allot;

                cfg->num_queues++;
            }
        }
    }

    fclose(file);
    return 0;
}

int main(int argc, char *argv[])
{
    char *input_file = NULL;
    char *algorithm = NULL;
    char *mlfq_config_file = NULL;
    int quantum = 30; // default time quantum

    // Command-line argument parser
    for (int i = 1; i < argc; i++)
    {
        if (strncmp(argv[i], "--input=", 8) == 0)
        {
            input_file = argv[i] + 8;
        }
        else if (strncmp(argv[i], "--algorithm=", 12) == 0)
        {
            algorithm = argv[i] + 12;
        }
        else if (strncmp(argv[i], "--quantum=", 10) == 0)
        {
            quantum = atoi(argv[i] + 10);
        }
        else if (strncmp(argv[i], "--mlfq-config=", 14) == 0)
        {
            mlfq_config_file = argv[i] + 14;
        }
    }

    if (!input_file || !algorithm)
    {
        fprintf(stderr, "Usage: %s --algorithm=<ALG> --input=<FILE>\n", argv[0]);
        return 1;
    }

    printf("Initializing %s Scheduler with workload: %s\n", algorithm, input_file);

    SchedulerState state;
    state.processes = parse_workload_file(input_file, &state.num_processes);
    if (!state.processes)
    {
        return 1; // Exit if parsing failed
    }

    state.current_time = 0;
    state.num_blocks = 0;
    init_gantt_chart(state.gantt_blocks, MAX_BLOCKS);

    printf("Successfully loaded %d processes.\n", state.num_processes);

    // Route to the correct algorithm based on the CLI argument

    if (strcmp(algorithm, "FCFS") == 0)
    {
        schedule_fcfs(&state);
    }
    else if (strcmp(algorithm, "RR") == 0)
    {
        schedule_rr(&state, quantum);
    }
    else if (strcmp(algorithm, "SJF") == 0)
    {
        schedule_sjf(&state);
    }
    else if (strcmp(algorithm, "STCF") == 0)
    {
        schedule_stcf(&state);
    }
    else if (strcmp(algorithm, "MLFQ") == 0)
    {
        if (!mlfq_config_file)
        {
            printf("MLFQ requires --mlfq-config=<FILE>\n");
            free(state.processes);
            return 1;
        }

        MLFQConfig cfg;

        if (load_mlfq_config(mlfq_config_file, &cfg) != 0)
        {
            free(state.processes);
            return 1;
        }

        schedule_mlfq(&state, &cfg);
    }
    else
    {
        printf("Algorithm %s is pending Student B's implementation!\n", algorithm);
        free(state.processes);
        return 1;
    }

    // Call Student A's Week 3 Comparative Table
    print_comparative_table(state.processes, state.num_processes, algorithm);

    // Clean up memory before exit to prevent leaks
    free(state.processes);
    return 0;
}