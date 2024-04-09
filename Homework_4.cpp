//Alex Kaylor (GLB116)
//HW 4 Project

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

struct Event{
	int processID;
	float service_time;
	float time;
	std:: string type;
};

Process* log_generator(int lambda, float avgServiceTime){
    int count = 10000;
    float cumulative_arrival_time = 0.00;
    float cumulative_service_time = 0.00;
    srand(time(NULL));
	Process* process_log = new Process[count];
	
    for (int i = 0; i < count; i++)
    {
        int process_id = i+1;
        
        //variables for calculations
        float random1 = (float)rand() / ((float)RAND_MAX);
        float random2 = (float)rand() / ((float)RAND_MAX);
		
		
		//calculate arrival_time
        float arrival_time = ((-logf(1.00-random1)) / lambda);
        cumulative_arrival_time += arrival_time;
        
        //calculate requested_service_time
        float requested_service_time = ((-logf(1.00-random2)) / avgServiceTime);
        cumulative_service_time += requested_service_time;
		
		//store these calculations in the process_log array
		process_log[i].arrival_time = cumulative_arrival_time;
        process_log[i].service_time = requested_service_time;
	}
	return process_log;
}

int main() {
	//get inputs
	int count = 10000;
	int lambda;
	float avgServiceTime;
	
	//delcarations for metrics
	float sum_turnaround_time;
	float sum_time_cpu_is_idle;
	float sum_processes_ready;
    float previousEventClock;
    float clock_at_last_ready_change;
	int temp;
	
	std:: cout<< "Enter average rate of arrival (lambda)" << std::endl;
	std:: cin >> lambda;
	
	std:: cout<< "Enter average service time" << std::endl;
	std:: cin >> avgServiceTime;
	Process* process_log;
	process_log = log_generator(lambda, avgServiceTime);

	std::vector<Event> events;
	
	//populate events vector
	for (int i = 0; i < count; i++){
		Process p = process_log[i];
		Event e;
		e.processID = i+1;
		e.time = p.arrival_time;
		e.service_time = p.service_time;
		e.type = "Arrival";
		events.push_back(e);
	}
	
	
	std::vector<int> ready;
	bool cpu_is_busy = false;
	float clock = 0;
	
	//iterate through events
	while (events.size()>0){
		Event p = *events.begin();
		previousEventClock = clock;
		clock_at_last_ready_change = 0;
		clock = p.time;
		if (p.type == "Arrival"){

			if (cpu_is_busy){
				ready.push_back(p.processID);
				temp = ready.size();
				sum_processes_ready += temp * (clock - clock_at_last_ready_change);
			}
			else{
				//summation for utilization when CPU switches off idle
				sum_time_cpu_is_idle += clock - previousEventClock;
				cpu_is_busy = true;
				Event e;
				e.processID = 0;
				e.time = clock + p.service_time;
				e.service_time = 0;
				e.type = "Completion";
			}
		}
		else {
			//add to the turnaround time summation
			Process temp_process = process_log[p.processID];
			sum_turnaround_time += (clock - temp_process.arrival_time);
			if (ready.size() == 0) {
				cpu_is_busy = false;
			}
			else {
				int processID = *ready.begin();
				temp = ready.size();
				sum_processes_ready += temp * (clock - clock_at_last_ready_change);
				clock_at_last_ready_change = clock;
				ready.erase(ready.begin());
				Process process = process_log[processID];
				Event newEvent;
				newEvent.processID = processID;
				newEvent.time = clock + process.service_time;
				newEvent.type = "Completion";
				events.push_back(newEvent);
				
				sort(events.begin(), events.end(), [](Event &a, Event &b){ return a.time < b.time; });
				
			}
		}
		events.erase(events.begin());
	}
	
	//output
	std:: cout<< "Average Turnaround time: " << (sum_turnaround_time /10000) << std::endl
			  << "Throughput: " << (count/clock) << std::endl
			  << "CPU Utilization: " << (((clock - sum_time_cpu_is_idle) / clock)*100) << std::endl
			  << "Average number of processes in ready queue: " << (sum_processes_ready / clock) << std::endl;
}
