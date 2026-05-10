#include "scheduler.h"

/* * ARCHITECTURE NOTE:
 * The centralized simulate_scheduler() framework was deprecated and removed 
 * to resolve architectural mixed-responsibilities (fixes #...).
 * * Each scheduling algorithm (FCFS, SJF, STCF, RR, MLFQ) now actively 
 * manages its own discrete time loop and Ready Queue to allow for 
 * highly specific preemption and prioritization logic.
 */