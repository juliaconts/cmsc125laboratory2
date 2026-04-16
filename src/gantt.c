#include "gantt.h"

// initializes the gantt chart
void init_gantt_chart(GanttBlock *blocks, int max_blocks)
{
    for (int i = 0; i < max_blocks; i++)
    {
        blocks[i].pid[0] = '\0';
        blocks[i].start_time = 0;
        blocks[i].duration = 0;
    }
}

// record the gantt chart
void record_gantt(GanttBlock *blocks, int *num_blocks, int max_blocks, const char *pid, int start_time, int duration)
{
    if (*num_blocks > 0 && strcmp(blocks[*num_blocks - 1].pid, pid) == 0)
    {
        // Extend previous block if same PID
        blocks[*num_blocks - 1].duration += duration;
    }
    else
    {
        if (*num_blocks >= max_blocks)
        {
            // Optional: Print warning why chart is cut short
            static int warned = 0;
            if (!warned)
            {
                fprintf(stderr, "\nWarning: Gantt chart capacity reached. Some blocks omitted.\n");
                warned = 1;
            }
            return;
        }
        // Add new block
        strncpy(blocks[*num_blocks].pid, pid, 15);
        blocks[*num_blocks].pid[15] = '\0';
        blocks[*num_blocks].start_time = start_time;
        blocks[*num_blocks].duration = duration;
        (*num_blocks)++;
    }
}

// prints the gantt chart
void print_gantt_chart(GanttBlock *blocks, int num_blocks)
{
    printf("\n=== Gantt Chart ===\n");

    // Decide scaling factor
    int scale = 1;
    int max_time = 0;
    for (int i = 0; i < num_blocks; i++)
    {
        if (blocks[i].start_time + blocks[i].duration > max_time)
            max_time = blocks[i].start_time + blocks[i].duration;
    }
    if (max_time > 100)
        scale = 10; // Each char = 10 time units

    // Print PID blocks
    for (int i = 0; i < num_blocks; i++)
    {
        printf("[");
        int chars = blocks[i].duration / scale;
        if (chars < 1)
            chars = 1;
        for (int j = 0; j < chars; j++)
            printf("%c", blocks[i].pid[0]);
        printf("]");
    }
    printf("\n");

    // Print scaled time markers
    printf("Time: ");
    for (int i = 0; i < num_blocks; i++)
    {
        printf("%d ", blocks[i].start_time);
    }
    // Print last time
    if (num_blocks > 0)
        printf("%d", blocks[num_blocks - 1].start_time + blocks[num_blocks - 1].duration);
    printf("\n");
}