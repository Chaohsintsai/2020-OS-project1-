#ifndef _PROCESS_H_
#define _PROCESS_H_

#include <sys/types.h>

#define CHILD_CPU 1
#define PARENT_CPU 0

/* Running one unit time */
#define UNIT_T()				\
{						\
	volatile unsigned long i;		\
	for (i = 0; i < 1000000UL; i++);	\
}						\

struct process {
	char name[32];
	int t_ready;
	int t_exec;
	pid_t pid;
};

int assign_cpu(int pid, int core);
int exec(struct process proc);
int block(int pid);
int wakeup(int pid);

#endif
