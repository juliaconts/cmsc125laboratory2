#include <stdio.h>
#include "metrics.h"

void print_comparative_table(Process *processes, int num_processes, const char *algo_name)
{
    float avg_tat = 0, avg_wt = 0, avg_rt = 0;

    // Calculate totals
    for (int i = 0; i < num_processes; i++)
    {
        avg_tat += processes[i].turnaround_time;
        avg_wt += processes[i].waiting_time;
        avg_rt += processes[i].response_time;
    }

    // Calculate averages
    if (num_processes > 0)
    {
        avg_tat /= num_processes;
        avg_wt /= num_processes;
        avg_rt /= num_processes;
    }

    // Print formatted ASCII table
    printf("\n---------------------- %s Performance Results ------------------- \n", algo_name);
    printf("| PID | Arrival | Burst | Finish | Turnaround | Waiting | Response |\n");
    printf("|-----|---------|-------|--------|------------|---------|----------|\n");
    for (int i = 0; i < num_processes; i++)
    {
        printf("| %-3s | %-7d | %-5d | %-6d | %-10d | %-7d | %-8d |\n",
               processes[i].pid,
               processes[i].arrival_time,
               processes[i].burst_time,
               processes[i].finish_time,
               processes[i].turnaround_time,
               processes[i].waiting_time,
               processes[i].response_time);
    }
    printf("|-----|---------|-------|--------|------------|---------|----------|\n");
    printf("| AVG |         |       |        |%-10.2f  | %-7.2f | %-8.2f |\n\n", avg_tat, avg_wt, avg_rt);
}