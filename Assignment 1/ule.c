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
#include <sys/time.h>

#define SIZE	20
#define TIMESLICE   100
#define ROUNDS  100
#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

//typedef enum {READY, WAITING, RUNNING} proc_state;

struct Node {
    int pid;
    double queueTime;
    //proc_state state = NULL;
    struct Node* next;
};

struct runTimes {
    double minRunTime;
    double avgRunTime;
    double maxRunTime;
    int counter;
};

struct Node* nextFront = NULL;
struct Node* nextRear = NULL;
struct Node* currentFront = NULL;
struct Node* currentRear = NULL;
struct Node* waitFront = NULL;
struct Node* waitRear = NULL;


struct runTimes* readyQueue = NULL;
struct runTimes* waitQueue = NULL;
struct runTimes* overhead = NULL;

//Retrieve clock time
double get_WallTime(){
    struct timeval tp;
    gettimeofday(&tp, NULL);
    return (double) (tp.tv_sec + tp.tv_usec/1000000.0);
}

void runTimeInit(){
    struct runTimes* readyTemp = (struct runTimes*)malloc(sizeof(struct runTimes));
    struct runTimes* waitTemp = (struct runTimes*)malloc(sizeof(struct runTimes));
    struct runTimes* overTemp = (struct runTimes*)malloc(sizeof(struct runTimes));

    readyQueue = readyTemp;
    waitQueue = waitTemp;
    overhead = overTemp;

    readyQueue->minRunTime = 999.0;
    readyQueue->avgRunTime = 0.0;
    readyQueue->maxRunTime = 0.0;
    readyQueue->counter = 0;

    waitQueue->minRunTime = 999.0;
    waitQueue->avgRunTime = 0.0;
    waitQueue->maxRunTime = 0.0;
    waitQueue->counter = 0;

    overhead->minRunTime = 999.0;
    overhead->avgRunTime = 0.0;
    overhead->maxRunTime = 0.0;
    overhead->counter = 0;
}

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

double isWaiting(int pid){
    struct Node* temp = waitFront;
    while(temp != NULL){
        if (temp->pid == pid){
            temp->queueTime = get_WallTime();
            return temp->queueTime;
        }
        temp = temp->next;
    }
    return 0.0;
}


struct Node* _popNode(struct Node* head, struct Node* Next, int pid){
    while(Next != NULL){
        if (Next->pid == pid){
            head->next = Next->next;
            return Next;
        }
        head = Next;
        Next = Next->next;
    }
    return NULL;
}

struct Node* popNode(struct Node* head, int pid){

    struct Node* Next = (struct Node*)malloc(sizeof(struct Node));
    if (head == NULL){
        return NULL;
    }
    if (head->pid == pid){
        Next = head;
        head = Next->next;
        return Next;
    }
    if (head->next == NULL){
        return NULL;
    }
    Next = head->next;
    return _popNode(head, Next, pid);
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
    double timeNow;
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

    struct Node* waitProc = popNode(waitFront, *pid);
    if (waitProc != NULL && waitProc->queueTime != 0.0){
        timeNow = get_WallTime();
        timeNow = (timeNow - (waitProc->queueTime));
        waitQueue->avgRunTime = timeNow;
        waitQueue->minRunTime = MIN(waitQueue->minRunTime, timeNow);
        waitQueue->maxRunTime = MAX(waitQueue->maxRunTime, timeNow);
        waitQueue->counter += 1;
        timeNow = 0.0;
        free(waitProc);
    }

    readyQueue->counter += 1;
    timeNow = get_WallTime();
    timeNow = (timeNow - (temp->queueTime));
    readyQueue->avgRunTime += timeNow;
    readyQueue->minRunTime = MIN(readyQueue->minRunTime, timeNow);
    readyQueue->maxRunTime = MAX(readyQueue->maxRunTime, timeNow);
    free(temp);
}

void Ready(int pid, int CPUtimeUsed) {
    // Informs the student's code that the process with id = pid has changed from the running state
    // or waiting state to the ready state
    // The process should be added to the ready queue

    double waiting = isWaiting(pid);
    overhead->counter += CPUtimeUsed;

    //Scan the queue in search of the PID. If found, return node, else, create it.
    struct Node* temp = (struct Node*)malloc(sizeof(struct Node));
    temp->pid = pid;
    temp->queueTime = get_WallTime();
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
    struct Node* temp = (struct Node*)malloc(sizeof(struct Node));
    temp->pid = pid;
    temp->queueTime = 0.0; //get_WallTime();
    temp->next = NULL;

    if (waitFront == NULL && waitRear == NULL){
        waitFront = waitRear = temp;
        printf("process %d waiting\n", pid);
        return;
    }
    waitRear->next = temp;
    waitRear = temp;

    printf("process %d waiting\n", pid);
}


void Terminate(int pid) {
    // Informs the student's code that the process with id = pid has terminated
    printf("process %d terminated\n", pid);
}

void runTimeAdjust(){

    readyQueue->avgRunTime = (readyQueue->avgRunTime / readyQueue->counter) * 1000;
    waitQueue->avgRunTime = (waitQueue->avgRunTime / waitQueue->counter) * 1000;
   // overhead->avgRunTime = (overhead->avgRunTime / overhead->counter) * 1000;

    readyQueue->minRunTime = (readyQueue->minRunTime * 1000);
    waitQueue->minRunTime = (waitQueue->minRunTime * 1000);
    overhead->minRunTime = (overhead->minRunTime * 1000);

    readyQueue->maxRunTime = (readyQueue->maxRunTime * 1000);
    waitQueue->maxRunTime = (waitQueue->maxRunTime * 1000);
    overhead->maxRunTime = (overhead->maxRunTime * 1000);

    overhead->avgRunTime = (overhead->maxRunTime - overhead->minRunTime) - overhead->counter;
}

int main() {
    runTimeInit();
    overhead->minRunTime = get_WallTime();
    // Simulate for 100 rounds, timeslice=100
    Simulate(ROUNDS, TIMESLICE);
    overhead->maxRunTime = get_WallTime();

    runTimeAdjust();
    printf("The average, min, and max time in msec that processes spent in ready queue was: %f, %f, %f\n", readyQueue->avgRunTime, readyQueue->minRunTime, readyQueue->maxRunTime);
    printf("The average, min, and max time in msec that processes spent responding to I/O are: %f, %f, %f\n", waitQueue->avgRunTime, waitQueue->minRunTime, waitQueue->maxRunTime);
    printf("The proportion of time spent on scheduler overheads was %f msec.\n", overhead->avgRunTime);
    return 0;
}
