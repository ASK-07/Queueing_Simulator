//Alex Kaylor (GLB116)
//HW 5 Project

//Imports
#include <iostream>
#include <cstdlib> 
#include <ctime>
#include <cmath>
#include <vector>
#include <algorithm>


struct Process {
	float arrival_time;
	float service_time;
};

struct Event {
	int processID = 0;
	float service_time = 0.0;
	float time = 0.0;
	std::string type;
	int CPU_assigned = -1;
};

struct CPU {
	int cpu_id = 0;
	std::string state = "idle";
	float sum_idle_time = 0;
	float idle_start_clock = 0;
	std::vector<int> ready;
	float last_ready_change = 0;
	float ready_size_sum = 0;

};

Process* log_generator(int lambda, float avgServiceTime) {
	int count = 10000;
	float cumulative_arrival_time = 0.00;
	float cumulative_service_time = 0.00;
	srand(time(NULL));
	Process* process_log = new Process[count];

	for (int i = 0; i < count; i++)
	{
		int process_id = i;

		//Generating random numbers, avoiding 1.0f to avoid error in calculation.
		float random1 = 1.0f;
		float random2 = 1.0f;
		while (random1 == 1.0f || random2 == 1.0f)
		{
			//variables for calculations
			random1 = (float)rand() / ((float)RAND_MAX);
			random2 = (float)rand() / ((float)RAND_MAX);
		}


		//calculate arrival_time
		float arrival_time = ((-logf(1.00 - random1)) / lambda);
		cumulative_arrival_time += arrival_time;

		//calculate requested_service_time
		float requested_service_time = ((-logf(1.00 - random2)) * avgServiceTime);
		cumulative_service_time += requested_service_time;

		//store these calculations in the process_log array
		process_log[i].arrival_time = cumulative_arrival_time;
		process_log[i].service_time = requested_service_time;
	}
	return process_log;
}

int main() {
	//delcarations for metrics
	int count = 10000;
	int lambda;
	int scenario;
	int number_CPU;
	float avgServiceTime;
	float last_ready_change = 0;
	float ready_size_sum = 0;
	float sum_turnaround_time = 0;
	float ready_change_sum = 0;
	int cpus_busy = 0;

	//inputs
	std::cout << "Enter average rate of arrival (lambda)" << std::endl;
	std::cin >> lambda;

	std::cout << "Enter average service time" << std::endl;
	std::cin >> avgServiceTime;

	std::cout << "Enter the scenario (1 or 2)" << std::endl;
	std::cin >> scenario;

	std::cout << "Enter the number of CPUs" << std::endl;
	std::cin >> number_CPU;

	std::vector<Event> events;

	Process* process_log;
	process_log = log_generator(lambda, avgServiceTime);

	//populate events vector
	for (int i = 0; i < count; i++) {
		Process p = process_log[i];
		Event e;
		e.processID = i + 1;
		e.time = p.arrival_time;
		e.service_time = p.service_time;
		e.type = "Arrival";
		events.push_back(e);
	}
	//initialize states of CPU's
	std::vector<CPU> CPUs;
	for (int i = 0; i < number_CPU; i++) {
		CPU c;
		c.cpu_id = i + 1;
		c.state = "Idle";
		c.sum_idle_time = 0;
		CPUs.push_back(c);
	}

	//scenario 1
	if (scenario == 1) {
		float clock = 0.00;
		srand(time(NULL));
		//iterate through events
		while (events.size() > 0) {
			Event p = *events.begin();
			clock = p.time;
			if (p.type == "Arrival") {
				Event e;
				e.CPU_assigned = rand() % number_CPU + 1;
				if (CPUs[(e.CPU_assigned - 1)].state == "Idle") {
					CPUs[(e.CPU_assigned - 1)].sum_idle_time += clock - CPUs[(e.CPU_assigned - 1)].idle_start_clock;
					CPUs[(e.CPU_assigned - 1)].state = "Busy";
					e.processID = p.processID;
					e.time = clock + p.service_time;
					e.service_time = p.service_time;
					e.type = "Completion";
					events.push_back(e);
					sort(events.begin(), events.end(), [](Event& a, Event& b) { return a.time < b.time; });
				}
				else {
					CPUs[(e.CPU_assigned - 1)].ready.push_back(p.processID);
				}
			}
			else {
				//add to the turnaround time summation
				Process temp_process = process_log[(p.processID - 1)];
				sum_turnaround_time += (clock - temp_process.arrival_time);

				int temp = p.CPU_assigned - 1;

				if (CPUs[temp].ready.size() == 0) {
					CPUs[temp].state = "Idle";
					CPUs[temp].idle_start_clock = clock;
				}
				else {
					int processID = *CPUs[temp].ready.begin();
					CPUs[temp].ready_size_sum += CPUs[temp].ready.size() * (clock - CPUs[temp].last_ready_change);
					CPUs[temp].ready.erase(CPUs[temp].ready.begin());
					CPUs[temp].last_ready_change = clock;
					Process process = process_log[processID];
					Event newEvent;
					newEvent.processID = processID;
					newEvent.time = clock + process.service_time;
					newEvent.type = "Completion";
					newEvent.CPU_assigned = temp + 1;
					events.push_back(newEvent);
					sort(events.begin(), events.end(), [](Event& a, Event& b) { return a.time < b.time; });

				}
			}
			events.erase(events.begin());
		}
		//output
		std::cout << "Average Turnaround time: " << (sum_turnaround_time / count) << std::endl
				  << "Throughput: " << (count / clock) << std::endl;
		for (CPU cpu : CPUs) {
			std::cout << "Average number of processes in ready queue of CPU  " << cpu.cpu_id << ": " << (cpu.ready_size_sum / clock) << std::endl
				      << "CPU " << cpu.cpu_id << " Utilization: " << (((clock - cpu.sum_idle_time) / clock) * 100) << std::endl;
		}
	}
	//Scenario 2
	if (scenario == 2) {
		std::vector<int> ready;
		float clock = 0.00;

		//iterate through events
		while (events.size() > 0) {
			Event p = *events.begin();
			clock = p.time;
			if (p.type == "Arrival") {
				cpus_busy = 0;
				for (CPU& cpu : CPUs) {
					if (cpu.state == "Idle") {
						
						cpu.sum_idle_time += clock - cpu.idle_start_clock;
						cpu.state = "Busy";
						Event e;
						e.processID = p.processID;
						e.time = clock + p.service_time;
						e.service_time = p.service_time;
						e.type = "Completion";
						e.CPU_assigned = cpu.cpu_id;
						events.push_back(e);
						sort(events.begin(), events.end(), [](Event& a, Event& b) { return a.time < b.time; });
					}
					else {
						cpus_busy++;
					}
				}
				if (cpus_busy == number_CPU) {
					ready.push_back(p.processID);
				}
			}
			else {
				//add to the turnaround time summation
				Process temp_process = process_log[(p.processID-1)];
				sum_turnaround_time += (clock - temp_process.arrival_time);

				int temp = p.CPU_assigned - 1;

				if (ready.size() == 0) {
					CPUs[temp].state = "Idle";
					CPUs[temp].idle_start_clock = clock;
				}
				else {
					int processID = *ready.begin();
					ready_size_sum += ready.size() * (clock - last_ready_change);
					ready.erase(ready.begin());
					last_ready_change = clock;
					Process process = process_log[processID];
					Event newEvent;
					newEvent.processID = processID;
					newEvent.time = clock + process.service_time;
					newEvent.type = "Completion";
					newEvent.CPU_assigned = temp + 1;
					events.push_back(newEvent);
					sort(events.begin(), events.end(), [](Event& a, Event& b) { return a.time < b.time; });

				}
			}
			events.erase(events.begin());
		}

		//output
		std::cout << "Average Turnaround time: " << (sum_turnaround_time / count) << std::endl
				  << "Throughput: " << (count / clock) << std::endl
				  << "Average number of processes in ready queue: " << (ready_size_sum / clock) << std::endl;
		for (CPU cpu : CPUs) {
			std::cout << "CPU " << cpu.cpu_id << " Utilization: " << (((clock - cpu.sum_idle_time) / clock) * 100) << std::endl;
		}
	}

}