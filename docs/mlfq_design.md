# MLFQ Design & Justification

## 1. Queue Architecture
Our Multi-Level Feedback Queue (MLFQ) consists of 3 priority levels:
* **Queue 0 (Highest Priority):** For short, interactive jobs.
* **Queue 1 (Medium Priority):** For jobs that need a bit more time.
* **Queue 2 (Lowest Priority):** For long, CPU-bound background jobs.

## 2. Time Quantums
* **Queue 0 Quantum:** 10ms
* **Queue 1 Quantum:** 20ms
* **Queue 2 Quantum:** 40ms
* *Justification:* We scale the time slices up as priority drops. This ensures that I/O-bound interactive tasks get rapid responses in Queue 0, while heavy CPU-bound tasks in Queue 2 get longer, uninterrupted bursts to do their math without constantly suffering context switch overhead.

## 3. Priority Boosting
* **Boost Parameter (S):** 100ms
* *Justification:* To prevent starvation (where a long job in Queue 2 never runs because short jobs keep arriving), we sweep all processes back to Queue 0 every 100ms. This guarantees CPU-bound jobs will eventually get CPU time, and also allows jobs that *used* to be CPU-bound but are now interactive to move back up.

## 4. Rule 4 Compliance (No BurstTime)
Our implementation strictly monitors how long a process *actually* runs. If a process uses up its entire time allotment at its current level, it is demoted. It does not use the parsed `burst_time` variable to make scheduling decisions.