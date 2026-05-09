# CMSC 125 Lab 2: CPU Scheduling

## Group Members
- Junel O. Arellano
- Julia Louise M. Contreras

## Description
A CPU Scheduling simulator that implements multiple classical CPU scheduling algorithms used in operating systems. The simulator reads process workload files, runs different CPU scheduling algorithms, and generates Gantt charts and performance metrics such as turnaround time, waiting time, response time, context switches, and queue behavior.

## Compilation and usage instructions
1. Compilation
    - Compile all source files using GCC: 

        ```
        gcc *.c -o/schedsim
        ```
    - With warnings enabled

        ```
        gcc -Wall -Wextra *.c/schedsim
        ```

2. Usage
    - General command format:
        ```
        ./schedsim--algorithm=<ALG> --input=<WORKLOAD_FILE>
        ```
    - Supported algorithms:
        
        1. FCFS
        2. SJF
        3. STCF
        4. RR
        5. MLFQ

    - Additional arguments
        
        1.  For Round Robin:
            `--quantum=<TIME>`
        2. For MLFQ Configuration:
            `--mlfq-config=<FILE>`
    - Example Commands
        1. FCFS
            ```
            ./schedsim --algorithm=FCFS --input=tests/workload.txt
            ```
        2. SJF
            ```
            ./schedsim --algorithm=SJF --input=tests/workload.txt
            ```
        3. STCF
            ```
            ./schedsim --algorithm=STCF --input=tests/workload.txt
            ```
        4. Round Robin
            ```
            ./schedsim --algorithm=RR --input=tests/workload.txt --quantum=30
            ```
        5. MLFQ
            ```
            ./schedsim --algorithm=MLFQ --mlfq-config=tests/mlfq_config.txt --input=tests/workload1.txt
            ```

## Implemented features and algorithms

### Implemented Scheduling Algorithms
1. First Come First Serve (FCFS)
    - Non-preemptive scheduling
    - Executes processes based on arrival order
    - Includes idle CPU tracking and Gantt chart support

2. Shortest Job First (SJF)
    - Non-preemptive shortest burst selection
    - Uses a ready queue with shortest-job dequeue logic
    - Computes turnaround, waiting, and response times

3. Shortest Time to Completion First (STCF)
    - Preemptive version of SJF
    - Supports process preemption and context switch tracking
    - Includes execution trace messages and average response time analysis

4. Round Robin (RR)
    - Time-sharing scheduler with configurable quantum
    - Tracks context switches and response times
    - Uses FIFO ready queue management

5. Multi-Level Feedback Queue (MLFQ)
    -  Multiple priority queues
    - Dynamic demotion and priority boosting
    - Configurable quantum, allotment, and boost period
    - Includes interactive and long-running process analysis

### Additional Features
- Dynamic workload file parsing with validation
- ASCII Gantt chart visualization
- Comparative performance metrics table
- Queue abstraction using linked lists
- Context switch tracking
- Idle CPU handling
- Priority boosting for MLFQ
- Input validation and bounded string parsing
- Dynamic memory allocation for workload expansion

## Example usage commands with expected output

## Known limitations or assumptions
- Maximum number of processes is limited to 64 by `MAX_PROCESSES`.
- Maximum number of MLFQ queues is limited to 10.
- Gantt chart visualization is text-based only.
- Some algorithms assume valid and properly formatted workload files.
- The simulator uses discrete-time simulation rather than real-time scheduling.
- Scheduling decisions occur at integer time ticks only.
- Context switch overhead is not simulated.
- The scheduler currently runs on a single CPU core simulation.
- Large workloads may produce very long Gantt chart outputs despite scaling support.
- The generic `simulate_scheduler()` framework remains partially unused because individual algorithms implement their own simulation loops.

## File Structure
```
Laboratory2
|-- Makefile
|-- README.md
+-- screenshots/         # Screenshots of working system
|-- include/
|   |-- gantt.h
|   |-- metrics.h
|   |-- mlfq.h
|   |-- process.h
|   |-- scheduler.h
|   |-- utils.c          
|-- src/
|   |-- main.c           # Program entry point
|   |-- process.c        # Process structure and initialization
|   |-- scheduler.c      # Core scheduler state and prototypes
|   |-- fcfs.c           # FCFS Implementation
|   |-- sjf.c            # SJF Implementation
|   |-- stcf.c           # STCF Implementation
|   |-- rr.c             # RR Implementation
|   |-- mlfq.c           # MLFQ Implementation
|   |-- utils.c          # Queue operatioins and helper functions
|   |-- metrics.c        # Gantt chart generation
|   |-- gantt.c          # Metrics calculation and reporting
|-- tests/
|   |-- workload1.txt          
|   |-- mlfq_config.txt       
|   |-- test_suite.sh    
+-- docs/
    +-- mlfq_design.md              # Design justifications
```