/*
 *	Assg2.h
 *
 *	Header file for CSCI 460 memory management exercise
 *
 *	David Bover, Computer Science, WWU, July 2009
 */

void Terminate(int pid);	// inform student code that the process has terminated

int Access(int pid, int address, int write);	
					// process pid wants to access address
					// write access if write is non-zero
					// return 0 indicates process request not satisfied
