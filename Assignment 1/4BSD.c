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

enum proc_state{READY, WAITING, RUNNING, NEW};

struct Node {
    int pid;
    int priority;
    enum proc_state state;
    struct Node* next;
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

void priority_init(){
    int i;
    for (i=0; i < 17; i++){
        struct pNode* temp = (struct pNode*)malloc(sizeof(struct pNode));
        priority[i] = temp;
        priority[i]->front = NULL;
        priority[i]->rear = NULL;
    }
}

struct Node* popNode(int pid, int CPUTIME){
    struct Node* temp = (struct Node*)malloc(sizeof(struct Node));
    struct Node* requestedNode = (struct Node*)malloc(sizeof(struct Node));
    temp->next = priority[16]->front;

    if (priority[16]->front == NULL){
        return NULL;
    }

    while(temp->next != NULL) {
        if (temp->next == temp->next->next){
            return NULL;
        }
        if (temp->next->pid == pid){
            if ((CPUTIME == TIMESLICE) && temp->next->priority != 0){
                temp->next->priority = temp->next->priority - 1;
            }
            else if ((temp->next->state == WAITING) && temp->next->priority != 15){
                temp->next->priority = temp->next->priority + 1;
            }
            requestedNode = temp->next;
            temp->next = requestedNode->next;
            return requestedNode;
        }
        temp = temp->next;
    }
    return NULL;
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
    int i = 15;
    struct Node* temp = NULL;

    //perror("BEGIN while\n");
    while (i >= 0){
        //perror("IN while\n");
        if (priority[i]->front != NULL){
            //perror("IN if\n");
            temp = priority[i]->front;
            //perror("SET TEMP = Priority[i]->front\n");
            if (priority[i]->front == priority[i]->rear){
               // perror("IN if2\n");
               // priority[i]->front = priority[i]->rear = NULL;
            }else{
               // perror("IN else\n");
                priority[i]->front = temp->next;
            }
            //perror("OUT if\n");
            *pid = temp->pid;
            //perror("JUST SET THE pid about to change state\n");
            temp->state = RUNNING;
            //perror("Changed state about to put node in [16]\n");
            if (priority[16]->front == NULL && priority[16]->rear == NULL){
                priority[16]->front = priority[16]->rear = temp;
            }else{
                 priority[16]->rear->next = temp;
                priority[16]->rear = temp;
            }
            printf("Process %d dispatched\n", *pid);
            return;
        }
        i--;
    }
}

void Ready(int pid, int CPUtimeUsed) {
    // Informs the student's code that the process with id = pid has changed from the running state
    // or waiting state to the ready state
    // The process should be added to the ready queue


    //Scan the queue in search of the PID. If found, return node, else, create it.
    struct Node* temp = popNode(pid, CPUtimeUsed);

    if (temp == NULL){
        temp = (struct Node*)malloc(sizeof(struct Node));
        temp->pid = pid;
        temp->priority = 8;
    }
    temp->state = READY;
    temp->next = NULL;

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
    printf("process %d waiting\n", pid);
}


void Terminate(int pid) {
    // Informs the student's code that the process with id = pid has terminated
    struct Node* temp = (struct Node*)malloc(sizeof(struct Node));
    struct Node* terminatedNode = (struct Node*)malloc(sizeof(struct Node));
    temp->next = priority[16]->front;

    while(temp->next != NULL) {
        if (temp->next == temp->next->next){
            return;
        }
        if (temp->next->pid == pid){
            terminatedNode = temp->next;
            temp->next = terminatedNode->next;
            free(terminatedNode);
        }
        temp = temp->next;
    }
    printf("process %d terminated\n", pid);
}

int main() {

    priority_init();
    // Simulate for 100 rounds, timeslice=100
    Simulate(ROUNDS, TIMESLICE);
    return 0;
}
