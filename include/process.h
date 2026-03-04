#ifndef PROCESS_H
#define PROCESS_H

typedef struct {
    char pid[16];          // Process identifier [cite: 950]
    int arrival_time;      // When process arrives [cite: 952]
    int burst_time;        // Total CPU time required [cite: 714]
    
    // Simulation tracking fields
    int remaining_time;    // Decrements as process runs [cite: 1095]
    int start_time;        // Tracked for response time [cite: 1118]
    int finish_time;       // When completed [cite: 955]
    
    // Calculated metrics
    int turnaround_time;   // FT - Arrival Time [cite: 1114]
    int waiting_time;      // Time spent waiting [cite: 956]
    int response_time;     // Start time - Arrival time [cite: 1119]
    
    // MLFQ specific fields
    int priority;          // Queue level [cite: 957]
    int time_in_queue;     // Allotment tracking [cite: 958]
} Process;

#endif