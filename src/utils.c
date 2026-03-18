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

Process* dequeue_shortest(Queue *q) {
    if (q->head == NULL) return NULL;

    Node *prev = NULL, *curr = q->head;
    Node *min_prev = NULL, *min_node = q->head;

    // Scan the queue for the shortest remaining time
    while (curr != NULL) {
        if (curr->process->remaining_time < min_node->process->remaining_time) {
            min_node = curr;
            min_prev = prev;
        } else if (curr->process->remaining_time == min_node->process->remaining_time) {
            // Tie-breaker: whoever arrived first wins
            if (curr->process->arrival_time < min_node->process->arrival_time) {
                min_node = curr;
                min_prev = prev;
            }
        }
        prev = curr;
        curr = curr->next;
    }

    // Unlink the node with the shortest time
    if (min_prev == NULL) {
        q->head = min_node->next; // Shortest was at the head
    } else {
        min_prev->next = min_node->next; // Shortest was in the middle/end
    }

    if (min_node == q->tail) {
        q->tail = min_prev; // Update tail if we removed the last item
    }

    Process *p = min_node->process;
    free(min_node);
    q->size--;
    return p;
}
