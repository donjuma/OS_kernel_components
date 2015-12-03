/*
 *  Header file FileSysAPI.h
 *
 *  File system API for CSCI 460 Assignment 3
 *
 *  David Bover, WWU, August 2008
 */

int CSCI460_Format ( );                                     // Formats the file system

int CSCI460_Write ( char *FileName, int Size, char *Data);  // Writes entire file

int CSCI460_Read (  char *FileName, int MaxSize, char *Data);  // Reads entire file

int CSCI460_Delete( char *Filename);                        // Deletes the file
