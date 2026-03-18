#ifndef UTILS_H
#define UTILS_H

#include "process.h"
#include <stdbool.h>

// Linked list node for the ready queue
typedef struct Node {
    Process *process;
    struct Node *next;
} Node;

// The Queue structure
typedef struct {
    Node *head;
    Node *tail;
    int size;
} Queue;

// Queue operations
void init_queue(Queue *q);
void enqueue(Queue *q, Process *p);
Process* dequeue(Queue *q);
Process* dequeue_shortest(Queue *q);
bool is_empty(Queue *q);
bool all_complete(Process *processes, int num_processes);

#endif