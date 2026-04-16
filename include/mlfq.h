#ifndef MLFQ_H
#define MLFQ_H

#define MAX_QUEUES 10

typedef struct
{
    int num_queues;
    int time_quantum[MAX_QUEUES];
    int allotment[MAX_QUEUES];
    int boost_period;
} MLFQConfig;

#endif