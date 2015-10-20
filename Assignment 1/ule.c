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
#define TIMESLICE   100
#define ROUNDS  15

//typedef enum {READY, WAITING, RUNNING} proc_state;

struct Node {
    int pid;
    //proc_state state = NULL;
    struct Node* next;
};

struct Node* nextFront = NULL;
struct Node* nextRear = NULL;
struct Node* currentFront = NULL;
struct Node* currentRear = NULL;

void swapQueue(){
    printf("Swapping queues\n");
    struct Node* tempFront = NULL;
    struct Node* tempRear = NULL;

    tempFront = nextFront;
    tempRear = nextRear;
    nextFront = currentFront;
    nextRear = currentRear;
    currentFront = tempFront;
    currentRear = tempRear;
}

void NewProcess(int pid) {
// Informs the student's code that a new process has been created, with process id = pid
// The new process should be added to the ready queue
	printf("new process %d created\n", pid);
	Ready(pid, 0);
}

void Dispatch(int *pid) {
// Requests the pid of the process to be changed to the running state
// The process should be removed from the ready queue
    *pid = 0;

    struct Node* temp = currentFront;

    if (currentFront == NULL){
        swapQueue();
        temp = currentFront;
        if (currentFront == NULL){
            //If both current and next are empty
            return;
        }
    }

    *pid = temp->pid;
	printf("Process %d dispatched\n", *pid);
    if (currentFront == currentRear){
        currentFront = currentRear = NULL;
    }
    else{
        currentFront = currentFront->next;
    }
    free(temp);
}

void Ready(int pid, int CPUtimeUsed) {
// Informs the student's code that the process with id = pid has changed from the running state
// or waiting state to the ready state
// The process should be added to the ready queue


    //Scan the queue in search of the PID. If found, return node, else, create it.
    struct Node* temp = (struct Node*)malloc(sizeof(struct Node));
    temp->pid = pid;
    //temp->proc_state = READY;
    temp->next = NULL;

    if (CPUtimeUsed < TIMESLICE){
        //Use the current queue.
        if (currentFront == NULL && currentRear == NULL){
            currentFront = currentRear = temp;
            printf("Process %d added to front of current queue after using %d msec\n", pid, CPUtimeUsed);
            return;
        }
        currentRear->next = temp;
        currentRear = temp;
        printf("Process %d added to current queue after using %d msec\n", pid, CPUtimeUsed);
    }
    else{
        //Use the next queue.
        if (nextFront == NULL && nextRear == NULL){
            nextFront = nextRear = temp;
            printf("Process %d added to front of next queue after using %d msec\n", pid, CPUtimeUsed);
            return;
        }
        nextRear->next = temp;
        nextRear = temp;
        printf("Process %d added to next queue after using %d msec\n", pid, CPUtimeUsed);
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
	Simulate(ROUNDS, TIMESLICE);
    return 0;
}
