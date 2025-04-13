# CPU Scheduling Simulation

This project simulates CPU scheduling using an event-driven model to evaluate performance under different multi-core scenarios. It generates synthetic process logs using exponential distributions and measures key system metrics.

## Files

- `cpu_scheduler_simulation.cpp`: The main simulation logic.
- `process_log_generator.h`: A header that generates synthetic process logs used in the simulation.

---

## 📋 How It Works

### Process Log Generation

Processes are generated with:
- **Arrival times**: Exponentially distributed based on a user-specified arrival rate (λ).
- **Service times**: Exponentially distributed based on a user-specified average.

A total of **10,000 processes** are generated and fed into the simulation.

### Simulation Scenarios

The simulation supports **two scheduling scenarios**:

#### Scenario 1: Independent Queues per CPU
Each process is randomly assigned to a CPU and waits in that CPU’s queue. CPUs only serve from their own queue.

#### Scenario 2: Shared Ready Queue
All processes wait in a shared queue. CPUs pull from the queue as they become idle.

---

## 🧪 Metrics Tracked

The simulator calculates and prints:

- **Average turnaround time** (arrival → completion)
- **System throughput** (processes completed per unit time)
- **CPU utilization** (time spent processing vs total time)
- **Average ready queue size** (per-CPU or shared, depending on scenario)

---

## 🚀 Getting Started

### Compile

```bash
g++ -std=c++11 cpu_scheduler_simulation.cpp -o simulator
