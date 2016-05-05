/*
 *  CPU scheduler simulator
 *
 *  Omar Juma
 *  W01090035
 *  CSCI: 460 - Operating Systems
 *  WWU Computer Science
 *  October 21, 2015
 *
 */


#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include "Dispatcher.h"
#include "SchedSim.h"

#define TIMESLICE   100
#define ROUNDS  100
#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

enum proc_state{READY, WAITING, RUNNING, NEW};

struct Node {
    int pid;
    int priority;
    double queueTime;
    enum proc_state state;
    struct Node* next;
};

struct runTimes {
    double minRunTime;
    double avgRunTime;
    double maxRunTime;
    int counter;
};

struct pNode {
    struct Node* front;
    struct Node* rear;
};

struct pNode* priority[17];
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


void priority_init(){
    int i;
    for (i=0; i < 17; i++){
        struct pNode* temp = (struct pNode*)malloc(sizeof(struct pNode));
        priority[i] = temp;
        priority[i]->front = NULL;
        priority[i]->rear = NULL;
    }
}

double isWaiting(int pid){
    struct Node* temp = waitFront;
    while(temp != NULL){
        if (temp->pid == pid){
            temp->queueTime = get_WallTime();
            return temp->queueTime;
        }
        if (temp == temp->next){
            temp->next = NULL;
            return 0.0;
        }
        temp = temp->next;
    }
    return 0.0;
}

void updateState(struct Node* head, int pid, enum proc_state state){
    if (head == NULL){
        return;
    }
    if (head->pid == pid){
        head->state = state;
        return;
    }
    if (head->next == NULL){
        return;
    }
    updateState(head->next, pid, state);
}

struct Node* _popWait(struct Node* head, struct Node* Next, int pid){
    while(Next != NULL){
        if (Next->pid == pid){
            head->next = Next->next;
            if (head->next == NULL){
                waitRear = head;
            }
            return Next;
        }
        if (head == head->next){
            head->next = NULL;
            return NULL;
        }
        head = Next;
        Next = Next->next;
    }
    return NULL;
}

struct Node* popWait(struct Node* head, int pid){

    struct Node* Next = (struct Node*)malloc(sizeof(struct Node));
    if (head == NULL){
        return NULL;
    }
    if (head->pid == pid){
        Next = head;
        head = Next->next;
        if (waitFront == NULL){
            waitRear = NULL;
        }
        return Next;
    }
    if (head->next == NULL){
        return NULL;
    }
    Next = head->next;
    return _popWait(head, Next, pid);
}

struct Node* _popNode(struct Node* head, struct Node* Next, int pid){
    while(Next != NULL){
        if (Next->pid == pid){
            head->next = Next->next;
            if (head->next == NULL){
                priority[16]->rear = head;
            }
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
        priority[16]->front = priority[16]->front->next;
        if (priority[16]->front == NULL){
            priority[16]->rear = NULL;
        }
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
    int i = 15;
    struct Node* temp = NULL;

    while (i >= 0){
        if (priority[i]->front != NULL){
            temp = priority[i]->front;
            if (priority[i]->front == priority[i]->rear){
                priority[i]->front = priority[i]->rear = NULL;
            }else{
                priority[i]->front = temp->next;
            }
            *pid = temp->pid;
            temp->state = RUNNING;
            temp->next = NULL;
            if (priority[16]->front == NULL && priority[16]->rear == NULL){
                priority[16]->front = priority[16]->rear = temp;
            }else{
                priority[16]->rear->next = temp;
                priority[16]->rear = temp;
            }
            priority[16]->rear->next = NULL;
            printf("Process %d dispatched\n", *pid);

            struct Node* waitProc = popWait(waitFront, *pid);
            if (waitProc != NULL && waitProc->queueTime != 0.0){
                timeNow = get_WallTime();
                timeNow = (timeNow - (waitProc->queueTime));
                waitQueue->avgRunTime += timeNow;
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
            //free(temp);

            return;
        }
        i--;
    }
}

void Ready(int pid, int CPUtimeUsed) {
    // Informs the student's code that the process with id = pid has changed from the running state
    // or waiting state to the ready state
    // The process should be added to the ready queue

    double waiting = isWaiting(pid);
    overhead->counter += CPUtimeUsed;

    //Scan the queue in search of the PID. If found, return node, else, create it.
    struct Node* temp = popNode(priority[16]->front, pid);

    if(temp != NULL){

        if (CPUtimeUsed == 100 && temp->priority != 0){
            temp->priority--;
        }
        if (temp->state == WAITING && temp->priority != 15){
            temp->priority++;
        }

    }else{
        temp = (struct Node*)malloc(sizeof(struct Node));
        temp->pid = pid;
        temp->priority = 8;
    }
    temp->state = READY;
    temp->next = NULL;
    temp->queueTime = get_WallTime();


    if (priority[temp->priority]->front == NULL && priority[temp->priority]->rear == NULL){
        priority[temp->priority]->front = priority[temp->priority]->rear = temp;
        printf("Process %d added to front of priority queue %d after using %d msec\n", pid, temp->priority, CPUtimeUsed);
        return;
    }
    priority[temp->priority]->rear->next = temp;
    priority[temp->priority]->rear = temp;
    printf("Process %d added to end of priority queue %d after using %d msec\n", pid, temp->priority, CPUtimeUsed);
    return;
}

void Waiting(int pid) {
    // Informs the student's code that the process with id = pid has changed from the running state
    // to the waiting state
    updateState(priority[16]->front, pid, WAITING);
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
    struct Node* temp = popNode(priority[16]->front, pid);    //(struct Node*)malloc(sizeof(struct Node));
    //struct Node* terminatedNode = (struct Node*)malloc(sizeof(struct Node));
    //temp->next = priority[16]->front;
    free(temp);


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

    priority_init();
    runTimeInit();
    overhead->minRunTime = get_WallTime();
    // Simulate for 100 rounds, timeslice=100
    Simulate(ROUNDS, TIMESLICE);
    overhead->maxRunTime = get_WallTime();

    runTimeAdjust();
    printf("The average, min, and max time in msec that processes spent in ready queue was: %f, %f, %f\n", readyQueue->avgRunTime, readyQueue->minRunTime, readyQueue->maxRunTime);
    printf("The average, min, and max time in msec that processes spent responding to I/O are: %f, %f, %f\n", waitQueue->avgRunTime, waitQueue->minRunTime, waitQueue->maxRunTime);
    printf("The proportion of time spent on scheduler overheads was %f msec.\n", overhead->avgRunTime);

    printf("Done\n");
    return 0;
}
