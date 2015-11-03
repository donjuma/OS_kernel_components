/*
 *	Omar Juma
 *  W01090035
 *  November 2, 2015
 *	CSCI 460 Assignment 2 - Memory Management Simulator
 *	Least Frequently Used Implementation
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include "MemSim.h"

#define TABLESIZE	512 		// Process Table (Known PIDs)
#define MAXPROC		16		    // maximum number of processes in the system
#define PAGESIZE	4096		// system page size

// data structure for process hash table entries
typedef struct procstruct {
	int pid;
    int pagenumber;
    int ref;
    int dirty;
    int valid;
} proc;

proc* table[TABLESIZE];		// Inverse Page Table
int processTable[MAXPROC];  // Table of Known PIDs
int procCount = 0;		    // Number of processes
int previousReplaced = 0;   // The IPT index of the last replaced (paged out) entry.
int writePageFault = 0;     // The amount of pageFaults requiring write before read
int readPageFault = 0;      // The amount of PageFaults requiring only reading of page to be swapped
int accessCalls = 0;        // The amount of times the Access() function has been called.

void init_table(){
    int i, j;
    proc* temp;

    //Initialize array of known processes to zero
    for (i=0; i<MAXPROC; i++){
        processTable[i] = 0;
    }

    //Initialize all IPT entries to zero values and load struct into table
    for (j=0; j<TABLESIZE; j++){
        temp = (proc*)malloc(sizeof(proc));
        temp->pid = 0;
        temp->pagenumber = 0;
        temp->ref = 0;
        temp->dirty = 0;
        temp->valid = 0;
        table[j] = temp;
    }
}

//This function searches for a given pagenumber, otherwise triggers a pageFault
int find(int pid, int pagenumber, int write){
    int i;

    for(i = 0; i < TABLESIZE; i++){
        if (table[i]->valid == 1 && table[i]->pid == pid && table[i]->pagenumber == pagenumber){
            table[i]->ref++;
            table[i]->dirty = (table[i]->dirty | write); //Dont remove dirtyness if reading after write...
            return i;
        }
    }
    return -1;
}

//Find PID in list of known running PIDs
int findPID(int pid){
    int i;
    for (i=0; i < MAXPROC; i++){
        if (processTable[i] == pid){
            return pid;
        }
    }
    return 0;
}


//Remove PID from the known PID array AND set all page entries for PID: Valid = FALSE
void Remove(int pid) {
    int i;

    for (i=0; i<TABLESIZE; i++){
        if (table[i]->pid == pid){
            table[i]->valid = 0;
        }
    }
    for (i=0; i<MAXPROC; i++){
        if (processTable[i] == pid){
            processTable[i] = 0;
            procCount--;
        }
    }
}


//Register new process PID into list of known PIDs
int registerPID(int pid){
    int i;
    if (procCount >= MAXPROC){
        return -2;
    }else{
        for(i=0; i<MAXPROC; i++){
            if (processTable[i] == 0){
                processTable[i] = pid;
                procCount++;
                return pid;
            }
        }
    }
    return -1;
}

//Insert a given pagenumber into the IPT
int insert(int pid, int pagenumber, int write){
    int i;
    for(i = 0; i < TABLESIZE; i++){
        if (table[i]->valid == 0){
            table[i]->pid = pid;
            table[i]->pagenumber = pagenumber;
            table[i]->ref = 0;
            table[i]->dirty = write;
            table[i]->valid = 1;
            return i;
        }
    }
    return -1;
}

//This function called at a regular interval will iterativly half the reference count on every page entry.
void age(){
    int index;
    for (index = 0; index < TABLESIZE; index++){
        table[index]->ref = (table[index]->ref / 2);
    }
}

//Least recently used.
int replace(int pid, int pagenumber, int write){
    int i;
    int LFU = table[0]->ref;
    int index = 0;

    for (i=0; i<TABLESIZE; i++){
        if (table[i]->ref < LFU){
            LFU = table[i]->ref;
            index = i;
        }
    }

    //Record Statistics for pageFaults. Negation worked better than if-else...
    if (table[index]->dirty) writePageFault++;
    if (!table[index]->dirty) readPageFault++;

    table[index]->pid = pid;
    table[index]->valid = 1;
    table[index]->pagenumber = pagenumber;
    table[index]->dirty = write;
    table[index]->ref = 0;
    return index;
}

int Access(int pid, int address, int write) {
	int pagenumber;
    int offset;
    int framenumber;
    int physicalAddr;
    int n;

    accessCalls++;
    offset = address & 0xFFF;
    pagenumber = address >> 12;

    //Half the reference of each page table every so often
    if (accessCalls % 250 == 0){
        printf("Aging all reference counts on page table entries\n");
        age();
    }

    //First search if known PID, else create it...
    if (findPID(pid)){
        if ((framenumber = find(pid, pagenumber, write)) >= 0) {
            physicalAddr = ((framenumber << 12) | offset);
	    	printf("pid %d wants %s access to address %d on page %d. Given: %08x\n",
	               pid, (write) ? "write" : "read", address, pagenumber, physicalAddr);
            return physicalAddr;
        }
        else if ((framenumber = insert(pid, pagenumber, write)) >= 0){
            physicalAddr = ((framenumber << 12) | offset);
	        printf("pid %d wants %s access to address %d on page %d. Given: %08x\n",
	               pid, (write) ? "write" : "read", address, pagenumber, physicalAddr);
            return physicalAddr;
        }
        else{
            //Record Statistics for pageFaults. Negation worked better than if-else...

            framenumber = replace(pid, pagenumber, write);
            if (framenumber < 0){
                perror("He's Dead, Jim!\n");
                abort();
            }
            physicalAddr = ((framenumber << 12) | offset);
	    	printf("pid %d wants %s access to address %d on page %d. Given: %08x\n",
	               pid, (write) ? "write" : "read", address, pagenumber, physicalAddr);
            return physicalAddr;
        }
    }

    //The PID is not known. Either add it to list or reject the request
    else{
        if (procCount >= MAXPROC){
            printf("pid %d refused\n", pid);
            return 0;
        }
        else if ((registerPID(pid)) >= 0){
            //INSERT OR REPLACE
            if ((framenumber = insert(pid, pagenumber, write)) >= 0){
                physicalAddr = ((framenumber << 12) | offset);
                return physicalAddr;
            }
            else{
                framenumber = replace(pid, pagenumber, write);
                physicalAddr = ((framenumber << 12) | offset);
                return physicalAddr;
            }
            return 0;
        }else if ((registerPID(pid)) == -2){
            printf("PID: %d REJECTED!\nREASON: Process limit reached\n", pid);
        }else{
            perror("Bad stuff happened\nPID: %d, address: %d, write: %d\n");
            printf("Bad stuff happened\nPID: %d, address: %d, write: %d, procCount: %d\n", pid, address, write, procCount);
            return 0;
        }
    }
}

// called when process terminates
void Terminate(int pid) {
	printf("pid %d terminated\n", pid);
	Remove(pid);
}

int main() {
    init_table();
	printf("MMU simulation started\n");
	Simulate(1000);
	printf("MMU simulation completed\n");
    printf("Number of page faults requiring only reading of the page to be " \
            "swapped in: %d\n", readPageFault);
    printf("Number of page faults requiring both writing of the replaced page " \
            "and reading of the page to be swapped in: %d\n", writePageFault);
    printf("Total number of page faults: %d\n", readPageFault + writePageFault);
}
