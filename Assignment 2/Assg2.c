/*
 *	Assg3.c
 *
 *	Demo of CSCI 460 memory management simulator
 *
 *	David Bover, Computer Science, WWU, July 2009
 */

#include <stdio.h>
#include <stdlib.h>
#include "MemSim.h"

#define TABLESIZE	2048		// size of hash table of processes
#define MAXPROC		16		// maximum number of processes in the system
#define PAGESIZE	4096		// system page size

// data structure for process hash table entries
typedef struct procstruct {
	int pid;
    int pagenumber;
    int ref;
    int dirty;
    int valid;
	//struct procstruct* next;
} proc;

proc* table[TABLESIZE];		// process hash table
int processTable[MAXPROC];
int procCount = 0;		// number of processes


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

// look for pid in the hash table
// if pid found, return 1
// if pid not found and number of processes < MAXPROC, add pid to hash table and return 1
// if pid not found and number of processes >= MAXPROC, return 0
/*
int find(int pid) {
	int index = pid % TABLESIZE;
	proc* node = table[index];
	proc* prev = NULL;

	// look along the chain for this hash table index
	while (node != NULL && node->pid != pid) {
		prev = node;
		node = node->next;
	}

	// pid not found in hash table
	if (node == NULL) {
		if (procCount >= MAXPROC)
			return 0;		// too many processes

		else {				// add new process
			node = (proc*) malloc(sizeof(proc));
			node->pid = pid;
			node->next = NULL;
			procCount++;
			if (prev == NULL)
				table[index] = node;
			else
				prev->next = node;
		}
	}
	return 1;
}
*/

int find(int pid, int pagenumber){
    int i;

    for(i = 0; i < TABLESIZE; i++){
        if (table[i]->valid == 1 && table[i]->pid == pid && table[i]->pagenumber == pagenumber){
            return i+1; //Incase first entry returns index 0, the function would imply FALSE
        }
    }
    return 0;
}

int findPID(int pid){
    int i;
    for (i=0; i < MAXPROC; i++){
        if (processTable[i] == pid){
            return pid;
        }
    }
    return 0;
}

/*
// remove a pid from the hash table
void Remove(int pid) {
	int index = pid % TABLESIZE;
	proc* node = table[index];
	proc* prev = NULL;

	// look along the chain for this hash table index
	while (node != NULL && node->pid != pid) {
		prev = node;
		node = node->next;
	}

	// if pid found, remove it
	if (node != NULL) {
		if (prev == NULL)
			table[index] = node->next;
		else
			prev->next = node->next;
		free(node);
	}
}
*/

int registerPID(int pid){
    int i;
    if (procCount >= MAXPROC){
        return 0;
    }else{
        for(i=0; i<MAXPROC; i++){
            if (processTable[i] == 0){
                processTable[i] = pid;
                return pid;
            }
        }
    }
    return 0;
}

int insert(int pid, int pagenumber, int write){
    int i;
    for(i = 0; i < TABLESIZE; i++){
        if (table[i]->valid == 0){
            table[i]->pid = pid;
            table[i]->pagenumber = pagenumber;
            table[i]->ref = 1;
            table[i]->dirty = write;
            table[i]->valid = 1;
            return i+1;
        }
    }
    return 0;
}

int replace(int pid, int pagenumber, int write){
    return 1;
}

// called in response to a memory access attempt by process pid to memory address
int Access(int pid, int address, int write) {
	int pagenumber;
    int offset;
    int framenumber;
    int physicalAddr;
    int n;

    offset = address & 0xFFF;
    pagenumber = address >> 12;

    //First search if known PID, else create it...
    if (findPID(pid)){
        if (framenumber = find(pid, pagenumber)) {
            framenumber--; //We added one to the return value of find, decrement it now.
            physicalAddr = ((framenumber << 12) | offset);
	    	printf("pid %d wants %s access to address %d on page %d. Given: %08x\n",
	               pid, (write) ? "write" : "read", address, pagenumber, physicalAddr);
            return physicalAddr;
        }
        else if (framenumber = insert(pid, pagenumber, write)){
            framenumber--; //We added one to the return value of insert, decrement it now.
            physicalAddr = ((framenumber << 12) | offset);
	    	printf("pid %d wants %s access to address %d on page %d. Given: %08x\n",
	               pid, (write) ? "write" : "read", address, pagenumber, physicalAddr);
            return physicalAddr;
        }
        //Not a known PID and unable to insert into ipt. Must replace an entry...
        else{
            framenumber = replace(pid, pagenumber, write);
            if (!framenumber){
                perror("He's Dead, Jim!\n");
                abort();
            }
            framenumber--; //We added one to the return value of replace, decrement it now.
            physicalAddr = ((framenumber << 12) | offset);
	    	printf("pid %d wants %s access to address %d on page %d. Given: %08x\n",
	               pid, (write) ? "write" : "read", address, pagenumber, physicalAddr);
            return physicalAddr;
        }
    }else{
        if (procCount >= MAXPROC){
            printf("pid %d refused\n", pid);
            return 0;
        }
        else if (registerPID(pid)){
            //INSERT OR REPLACE
            printf("PLEASE register my PID... I'm so alone....\n");
            return 0;
        }else{
            perror("Bad stuff happened\n");
            return 0;
        }
    }
}

/*
	//	printf("pid %d wants %s access to address %d on page %d\n",
	//	       pid, (write) ? "write" : "read", address, address/PAGESIZE);
        printf("PID: %d,  ADDRESS: %08x   WRITE: %d\n", pid, address, write);
		return 1;
	} else {
		printf("pid %d refused\n", pid);
		return 0;
	}
}
*/


// called when process terminates
void Terminate(int pid) {
	printf("pid %d terminated\n", pid);
	procCount--;
//	Remove(pid);
}

int main() {
	//int i;

	// initialize the process hash table
	//for (i = 0; i < TABLESIZE; i++) table[i] = NULL;
    init_table();

	printf("MMU simulation started\n");
	Simulate(1000);
	printf("MMU simulation completed\n");
}
