/*
 *  Demonstration of CPU scheduler simulator
 *  using cheap, limited ready queue
 *
 *  David Bover, WWU Computer Science, June 2007
 *  updated October, 2015
 *
 */


#include <stdio.h>
#include <stdlib.h>
#include "Dispatcher.h"
#include "SchedSim.h"

#define SIZE	20
// a real cheap ready queue - don't try this at home, children!
int readyproc[SIZE];
int numready = 0;
int first = 0;
int last = 0;

void NewProcess(int pid) {
// Informs the student's code that a new process has been created, with process id = pid
// The new process should be added to the ready queue
	printf("new process %d created\n", pid);
	Ready(pid, 0);				// note: you need to invoke Ready()
}

void Dispatch(int *pid) {
// Requests the pid of the process to be changed to the running state
// The process should be removed from the ready queue
	*pid = 0;				// zero pid implies an empty ready queue
							// this should never happen, but ...
	if (numready > 0) {
		numready--;
		*pid = readyproc[last++];
		if (last >= SIZE) last = 0;
		printf("process %d dispatched\n", *pid);
	} 
}

void Ready(int pid, int CPUtimeUsed) {
// Informs the student's code that the process with id = pid has changed from the running state
// or waiting state to the ready state
// The process should be added to the ready queue
	if (numready < SIZE) {
		numready++;
		readyproc[first++] = pid;// if the ready queue fills, we start to lose pids
								// this is for demo purposes only!
		if (first >= SIZE) first = 0;
		printf("process %d added to ready queue after using %d msec\n", pid, CPUtimeUsed);
	}
}

void Waiting(int pid) {
// Informs the student's code that the process with id = pid has changed from the running state
// to the waiting state
	printf("process %d waiting\n", pid);
}


void Terminate(int pid) {
// Informs the student's code that the process with id = pid has terminated
	printf("process %d terminated\n", pid);
}

int main() {
	// Simulate for 100 rounds, timeslice=100
	Simulate(1000, 100);
    return 0;
}
