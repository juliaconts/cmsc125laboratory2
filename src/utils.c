#include <stdlib.h>
#include "utils.h"

void init_queue(Queue *q) {
    q->head = NULL;
    q->tail = NULL;
    q->size = 0;
}

void enqueue(Queue *q, Process *p) {
    Node *new_node = malloc(sizeof(Node));
    if (!new_node) return;
    new_node->process = p;
    new_node->next = NULL;

    if (q->tail == NULL) {
        q->head = new_node;
        q->tail = new_node;
    } else {
        q->tail->next = new_node;
        q->tail = new_node;
    }
    q->size++;
}

Process* dequeue(Queue *q) {
    if (q->head == NULL) return NULL;

    Node *temp = q->head;
    Process *p = temp->process;
    
    q->head = q->head->next;
    if (q->head == NULL) {
        q->tail = NULL;
    }
    
    free(temp);
    q->size--;
    return p;
}

bool is_empty(Queue *q) {
    return q->head == NULL;
}

// Helper to check if the simulation is totally finished
bool all_complete(Process *processes, int num_processes) {
    for (int i = 0; i < num_processes; i++) {
        if (processes[i].remaining_time > 0) {
            return false; 
        }
    }
    return true;
}