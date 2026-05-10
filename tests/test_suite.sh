#!/bin/bash

echo "=================================================="
echo "      CMSC 125 CPU Scheduler Test Suite"
echo "=================================================="

# --------------------------------------------------
# Compile Project
# --------------------------------------------------

echo ""
echo "[1] Compiling Project..."

make clean > /dev/null 2>&1
make > /dev/null 2>&1

if [ $? -ne 0 ]; then
    echo "Compilation FAILED"
    exit 1
fi

echo "Compilation PASSED"

# --------------------------------------------------
# Lecture Verification Tests
# --------------------------------------------------

echo ""
echo "[2] Lecture Verification Tests"

# FCFS
./schedsim --algorithm=FCFS --input=tests/quiz3.txt > /tmp/fcfs.txt

if grep -q "515" /tmp/fcfs.txt; then
    echo "FCFS average TT: PASS"
else
    echo "FCFS average TT: FAIL (expected 515)"
fi

# SJF
./schedsim --algorithm=SJF --input=tests/quiz3.txt > /tmp/sjf.txt

if grep -q "461" /tmp/sjf.txt; then
    echo "SJF average TT: PASS"
else
    echo "SJF average TT: FAIL (expected 461)"
fi

# STCF
./schedsim --algorithm=STCF --input=tests/quiz3.txt > /tmp/stcf.txt

if grep -q "393" /tmp/stcf.txt; then
    echo "STCF average TT: PASS"
else
    echo "STCF average TT: FAIL (expected 393)"
fi

# RR
./schedsim --algorithm=RR --input=tests/quiz3.txt --quantum=30 > /tmp/rr.txt

if [ $? -eq 0 ]; then
    echo "RR execution: PASS"
else
    echo "RR execution: FAIL"
fi

# --------------------------------------------------
# Unit Testing
# --------------------------------------------------

echo ""
echo "[3] Unit Testing"

# Process arrival
./schedsim --algorithm=FCFS --input=tests/process_arrival.txt > /dev/null

if [ $? -eq 0 ]; then
    echo "Process arrival test: PASS"
else
    echo "Process arrival test: FAIL"
fi

# Queue operations
./schedsim --algorithm=SJF --input=tests/queue_sorting.txt > /dev/null

if [ $? -eq 0 ]; then
    echo "Queue sorting test: PASS"
else
    echo "Queue sorting test: FAIL"
fi

# Metrics calculation
./schedsim --algorithm=FCFS --input=tests/metrics_small.txt > /dev/null

if [ $? -eq 0 ]; then
    echo "Metrics calculation test: PASS"
else
    echo "Metrics calculation test: FAIL"
fi

# MLFQ transitions
./schedsim \
--algorithm=MLFQ \
--input=tests/mlfq_transition.txt \
--mlfq-config=tests/mlfq_config.txt > /dev/null

if [ $? -eq 0 ]; then
    echo "MLFQ transition test: PASS"
else
    echo "MLFQ transition test: FAIL"
fi

# --------------------------------------------------
# Edge Case Testing
# --------------------------------------------------

echo ""
echo "[4] Edge Case Testing"

# Single process
./schedsim --algorithm=FCFS --input=tests/single.txt > /dev/null

if [ $? -eq 0 ]; then
    echo "Single process test: PASS"
else
    echo "Single process test: FAIL"
fi

# Simultaneous arrivals
./schedsim --algorithm=STCF --input=tests/simultaneous.txt > /dev/null

if [ $? -eq 0 ]; then
    echo "Simultaneous arrivals test: PASS"
else
    echo "Simultaneous arrivals test: FAIL"
fi

# Zero burst
./schedsim --algorithm=FCFS --input=tests/zero_burst.txt > /dev/null

if [ $? -eq 0 ]; then
    echo "Zero burst time test: PASS"
else
    echo "Zero burst time test: FAIL"
fi

# Large workload
./schedsim --algorithm=RR --input=tests/large_workload.txt --quantum=20 > /dev/null

if [ $? -eq 0 ]; then
    echo "Large workload test: PASS"
else
    echo "Large workload test: FAIL"
fi

# Identical bursts
./schedsim --algorithm=SJF --input=tests/identical_bursts.txt > /dev/null

if [ $? -eq 0 ]; then
    echo "Identical burst times test: PASS"
else
    echo "Identical burst times test: FAIL"
fi

# Staircase arrivals
./schedsim --algorithm=STCF --input=tests/staircase.txt > /dev/null

if [ $? -eq 0 ]; then
    echo "Staircase arrivals test: PASS"
else
    echo "Staircase arrivals test: FAIL"
fi

# --------------------------------------------------
# Performance Analysis Tests
# --------------------------------------------------

echo ""
echo "[5] Performance Analysis"

# All short jobs
./schedsim \
--algorithm=MLFQ \
--input=tests/all_short.txt \
--mlfq-config=tests/mlfq_config.txt > /dev/null

if [ $? -eq 0 ]; then
    echo "All short jobs workload: PASS"
else
    echo "All short jobs workload: FAIL"
fi

# All long jobs
./schedsim \
--algorithm=MLFQ \
--input=tests/all_long.txt \
--mlfq-config=tests/mlfq_config.txt > /dev/null

if [ $? -eq 0 ]; then
    echo "All long jobs workload: PASS"
else
    echo "All long jobs workload: FAIL"
fi

# Mixed workload
./schedsim \
--algorithm=MLFQ \
--input=tests/mixed.txt \
--mlfq-config=tests/mlfq_config.txt > /dev/null

if [ $? -eq 0 ]; then
    echo "Mixed workload test: PASS"
else
    echo "Mixed workload test: FAIL"
fi

# Bimodal workload
./schedsim \
--algorithm=MLFQ \
--input=tests/bimodal.txt \
--mlfq-config=tests/mlfq_config.txt > /dev/null

if [ $? -eq 0 ]; then
    echo "Bimodal workload test: PASS"
else
    echo "Bimodal workload test: FAIL"
fi

# I/O-bound simulation
./schedsim \
--algorithm=MLFQ \
--input=tests/io_bound.txt \
--mlfq-config=tests/mlfq_config.txt > /dev/null

if [ $? -eq 0 ]; then
    echo "I/O-bound simulation test: PASS"
else
    echo "I/O-bound simulation test: FAIL"
fi

# --------------------------------------------------
# Memory Leak Testing
# --------------------------------------------------

echo ""
echo "[6] Memory Leak Testing"

valgrind --leak-check=full \
         --error-exitcode=1 \
         ./schedsim --algorithm=FCFS --input=tests/quiz3.txt \
         > /dev/null 2>&1

if [ $? -eq 0 ]; then
    echo "Memory leak test: PASS"
else
    echo "Memory leak test: FAIL"
fi

# --------------------------------------------------

echo ""
echo "=================================================="
echo "           ALL TESTS COMPLETED"
echo "=================================================="