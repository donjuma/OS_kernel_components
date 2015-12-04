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
#include <string.h>
#include "Driver.h"

#define BYTES_PER_SECTOR    64
#define SECTORS             32 * 1024
#define MIN(a,b) (((a)<(b))?(a):(b))

typedef struct freelist{
    int lblock;
    int size;
    struct freelist* next;
} freelist;

typedef struct entry{
    char name[32];
    int size;
    struct inode* block;
    struct entry* next;
} entry;

typedef struct inode{
    int blocks[13];
    struct sinode* single;
    struct dinode* dub;
    struct tinode* triple;
} inode;

typedef struct sinode{
    int blocks[16];
} sinode;

typedef struct dinode{
    struct sinode* blocks[16];
} dinode;

typedef struct tinode{
    struct dinode* blocks[16];
} tinode;

struct freelist* freel = NULL;
struct entry* files = NULL;

int CSCI460_Format(){
    if (!DevFormat()){
        return 0;
    }

    if (freel != NULL){
        free(freel);
    }
    if (files != NULL){
        free(files);
    };
    struct freelist* freel = (struct freelist*)malloc(sizeof(struct freelist));
    //struct entry* files = (struct entry*)malloc(sizeof(struct entry));

    freel->lblock = 1;
    freel->size = SECTORS;
    freel->next = NULL;

    return 1;
}

int reserve(int size){
    struct freelist* temp = freel;
    int location;
    size = (size + BYTES_PER_SECTOR - 1)/BYTES_PER_SECTOR;
    if (temp == NULL){
        return 0;
    }
    while (temp != NULL){
        if (temp->size >= size){
            temp->size -= size;
            location = temp->lblock;
            temp->lblock += size;
            return location;
        }
        temp = temp->next;
    }
    return -1;
}

int fill_inode(struct entry* file, int size, char *Data, int location){
    //Size -1 ?
    char buffer[BYTES_PER_SECTOR+1];
    struct inode* node = file->block;
    int single = 0, dub = 0, triple = 0;
    int i = 0, n = 0, level = 0, temp = 0;
    int space = (size + BYTES_PER_SECTOR - 1)/BYTES_PER_SECTOR;

    if (space >  13){
        single = 1;
    }
    if (space > 29){
        dub = 1;
    }
    if (space > 269){
        triple = 1;
    }
    if (space > 4109){
        printf("ERROR: FileSize too large for inode subsystem\n");
        return 0;
    }

    n = MIN(space, 13);
    for (i = 0; i < n; i++){
        strncpy(buffer, Data+(i*BYTES_PER_SECTOR), BYTES_PER_SECTOR);
        if (!DevWrite(location+i, buffer)){
            printf("ERROR: Unknown error writing file to disk\n");
            return 0;
        }
        node->blocks[i] = location+i;
    }

    if (single){
        n = MIN(space, 29);
        for (i = 13; i < n; i++){
            strncpy(buffer, Data + (i*BYTES_PER_SECTOR), BYTES_PER_SECTOR);
            if (!DevWrite(location+i, buffer)){
                printf("ERROR: Unknown error writing file to disk\n");
                return 0;
            }
            node->single->blocks[i-13] = location + i;
        }
    }

    if (dub){
        n = MIN(space, 269);
        temp = (n-29);
        level = (temp + 16 - 1)/16;

        for (int j = 0; j<level; j++){
            for (i = 29+(16*j); i < 45+(j*16); i++){
                strncpy(buffer, Data + (i*BYTES_PER_SECTOR), BYTES_PER_SECTOR);
                if (!DevWrite(location+i, buffer)){
                    printf("ERROR: Unknown error writing file to disk\n");
                    return 0;
                }
                node->dub->blocks[level]->blocks[i-(29+(16*j))] = location + i;
            }
        }
    }
    return
}

int CSCI460_Write(char *FileName, int Size, char *Data){
    //TODO: If file exists overwrite it
    //TODO: Check if file is too large for inode system.
    int location = reserve(Size);
    int created = 0;
    int filled;
    struct entry* temp;
    struct entry* iterator;
    struct inode* filenode = (struct inode*)malloc(sizeof(struct inode));

    if (location < 1){
        return 0;
    }
    temp = (struct entry*)malloc(sizeof(struct entry));
    temp->name = FileName;
    temp->size = size;
    temp->block = filenode;
    temp->next = null;
    created = 1;

    if (files == NULL){
        //struct entry* files = (struct entry*)malloc(sizeof(struct entry));
        //files->name = filename;
        //files->size = size;
        //files->block = filenode;
        //file->next = null;
        //created = 1;
        files = temp;
    }
    else{
        iterator = files;
        while (iterator->next != NULL){
            iterator = iterator->next;
        }
        iterator->next = temp;
    }

    if (created){
        filled = fill_inode(temp, Size, Data, location);
        return filled;
    }
    else{
        return 0;
    }
    return 0;
}



int CSCI460_Read (char *FileName, int MaxSize, char *Data){
    struct entry* temp = (struct entry*)malloc(sizeof(struct entry));
    struct inode* filenode = (struct inode*)malloc(sizeof(struct inode));
    int match;
    int size;
    char buffer[BYTES_PER_SECTOR+1];
    temp = files;

    while(temp != NULL){
        if (!(match = strcmp(temp->name, FileName))){
            filenode = temp->block;
            size = temp->size;

            if(!(DevRead(filenode->blocks[1], buffer))){
                printf("ERROR: There was an error with reading your file\n");
                return 0;
            }
            printf("CONTENTS: %s\n", buffer);
        }
        temp = temp->next;
    }
    return 0;
}

int CSCI460_Delete( char *Filename){
    return 0;
}


















