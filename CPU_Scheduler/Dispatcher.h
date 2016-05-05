#ifndef _DISPATCH_H_
#define _DISPATCH_H_

/*	Header file Dispatcher.h
 *
 *	Defines functions to be provided by CSCI 460 students
 *
 *	David Bover, WWU, July, 2007
 *
 *	modified July, 2010
 */


void NewProcess(int pid);
// Informs the student's code that a new process has been created, with process id = pid
// The new process should be added to the ready queue

void Dispatch(int *pid);
// Requests from the student's code the pid of the process to be changed to the running state.
// The process should be removed from the ready queue

void Ready(int pid, int CPUtimeUsed);
// Informs the student's code that the process with id = pid has changed from the running state
// or waiting state to the ready state.  
// CPUtimeUsed is the CPU time that the process used.
// The process should be added to the ready queue

void Waiting(int pid);
// Informs the student's code that the process with id = pid has changed from the running state
// to the waiting state

void Terminate(int pid);
// Informs the student's code that the process with id = pid has terminated



#endif
