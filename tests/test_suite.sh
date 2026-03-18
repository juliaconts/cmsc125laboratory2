#!/bin/bash

echo "========================================="
echo "   Starting Automated Test Suite...      "
echo "========================================="

# Recompile to ensure latest changes are tested
make clean && make

# Define algorithms and the test workload
ALGORITHMS=ALGORITHMS=("SJF" "STCF")
WORKLOAD="tests/workload1.txt"

# Loop through each algorithm and run the simulator
for algo in "${ALGORITHMS[@]}"; do
    echo ""
    echo ">>> Running Algorithm: $algo <<<"
    ./schedsim --algorithm=$algo --input=$WORKLOAD
done

echo ""
echo "========================================="
echo "   All automated tests completed!        "
echo "========================================="