#define _GNU_SOURCE
#include "process.h"
#include "scheduler.h"
#include <stdlib.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sched.h>


static int t_last;


static int ntime;


static int running;


static int finish_cnt;


int cmp(const void *a, const void *b) {
	return ((struct process *)a)->t_ready - ((struct process *)b)->t_ready;
}


int next_process(struct process *proc, int amount, int policy)
{
	
	if (running != -1 && (policy == SJF || policy == FIFO))
		return running;

	int ret = -1;

	if (policy == PSJF || policy ==  SJF) {
		for (int i = 0; i < amount; i++) {
			if (proc[i].pid == -1 || proc[i].t_exec == 0)
				continue;
			if (ret == -1 || proc[i].t_exec < proc[ret].t_exec)
				ret = i;
		}
	}

	else if (policy == FIFO) {
		for(int i = 0; i < amount; i++) {
			if(proc[i].pid == -1 || proc[i].t_exec == 0)
				continue;
			if(ret == -1 || proc[i].t_ready < proc[ret].t_ready)
				ret = i;
		}
        }

	else if (policy == RR) {
		if (running == -1) {
			for (int i = 0; i < amount; i++) {
				if (proc[i].pid != -1 && proc[i].t_exec > 0){
					ret = i;
					break;
				}
			}
		}
		else if ((ntime - t_last) % 500 == 0)  {
			ret = (running + 1) % amount;
			while (proc[ret].pid == -1 || proc[ret].t_exec == 0)
				ret = (ret + 1) % amount;
		}
		else
			ret = running;
	}

	return ret;
}

int scheduling(struct process *proc, int amount, int policy)
{
	qsort(proc, amount, sizeof(struct process), cmp);

	
	for (int i = 0; i < amount; i++)
		proc[i].pid = -1;

	
	assign_cpu(getpid(), PARENT_CPU);
	
	wakeup(getpid());
	
	ntime = 0;
	running = -1;
	finish_cnt = 0;
	
	while(1) {
		
		if (running != -1 && proc[running].t_exec == 0) {
		

			waitpid(proc[running].pid, NULL, 0);
			running = -1;
			finish_cnt++;

			if (finish_cnt == amount)
				break;
		}


		for (int i = 0; i < amount; i++) {
			if (proc[i].t_ready == ntime) {
				proc[i].pid = exec(proc[i]);
				block(proc[i].pid);
				printf("%s %d\n", proc[running].name, proc[running].pid);
				fflush();

			}

		}

		int next = next_process(proc, amount, policy);
		if (next != -1) {
			// Context switch 
			if (running != next) {
				if(running !=-1){
					block(proc[running].pid);
				}
				wakeup(proc[next].pid);
				running = next;
				t_last = ntime;
			}
		}

		UNIT_T();
		if (running != -1)
			proc[running].t_exec--;
		ntime++;
	}

	return 0;
}
