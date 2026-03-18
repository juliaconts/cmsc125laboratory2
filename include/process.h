#ifndef PROCESS_H
#define PROCESS_H

typedef struct {
    char pid[16];          // Process identifier 
    int arrival_time;      // When process arrives 
    int burst_time;        // Total CPU time required 
    
    // Simulation tracking fields
    int remaining_time;    // Decrements as process runs 
    int start_time;        // Tracked for response time 
    int finish_time;       // When completed 
    
    // Calculated metrics
    int turnaround_time;   // FT - Arrival Time 
    int waiting_time;      // Time spent waiting 
    int response_time;     // Start time - Arrival time 
    
    // MLFQ specific fields
    int priority;          // Queue level 
    int time_in_queue;     // Allotment tracking 
} Process;



#endif