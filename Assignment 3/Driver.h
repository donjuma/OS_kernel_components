/*
 *  Header file Driver.h
 *
 *  Device driver for CSCI 460 Assignment 3
 *
 *  David Bover, WWU, August 2010
 */

#define BYTES_PER_SECTOR    64
#define SECTORS             32 * 1024

int DevFormat();                    // formats the device

int DevWrite (                      // writes one block to a specified sector
               int BlockNumber,     // Logical block number
               char *Data           // Data to be written
               );

int DevRead (                       // reads one block from a specified sector
              int BlockNumber,      // Logical block number
              char *Data            // Data received
              );

