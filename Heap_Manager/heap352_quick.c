// Omar Juma
// W01090035
// CSCI 352, ASSIGNMENT 1

#include <stdio.h>

#include "HeapTestEngine.h"

#define MIN(A,B) ((A)<(B)?(A):(B))
#define ALIGN(x) (((((x)-1)>>4)<<4)+16)
#define HEADER_SIZE 16
#define MIN_UNIT 16

#ifndef DEBUG
#define DEBUG 1
#endif

/*
typedef struct header *Header;

struct header {
    Header *next;
    unsigned size;

    size_t size;
    t_block next;
    int free;
};
*/
//void *base = NULL;
//void *root = NULL;

union header{
    struct {
        union header *next;
        unsigned size;
        //char data[1];  //Pointer to our payload
    } this;
    long x;
};
typedef union header Header;
Header *increase_heap(int size);
static Header base;
static Header *freep = NULL;
unsigned totalBlocks = 0;
unsigned totalFreed = 0;
struct Header *freeps[9];
int regular = 1;

void print_debug(Header *freep){
    Header *current = freep;
    int size;
    unsigned next_free, next_block;
    while (freep != NULL){
        size = current->this.size;
        next_free = current->this.next;
        next_block = (current + (MIN_UNIT * size));
        printf("Block at 0x%07x, size %d, next free at 0x%07x, next block at 0x%07x", current, size, next_free, next_block);
        if (next_free == next_block){
            printf("******");
        }
        printf("\n");
        current = current->this.next;
        if(current == freep){
            break;
        }
    }
    printf("\n\nBlocks allocated: %u, free: %u, total: %u\n", (totalBlocks-totalFreed), totalFreed, totalBlocks);
    return;
}


Header *find_first_block(Header *previous , Header *current, int size ){
    while (current && (current->this.size < size) ){
        if (current == freep){
            if ((current = increase_heap(size)) == NULL){
                return NULL;
            }
        }
        previous = current;
        current = current->this.next;
    }
    return (current);
}

void split(Header *current, int size){
    //size = size + HEADER_SIZE;
    current->this.size = current->this.size - size;
    current = current + size;
    current->this.size = size;
    /*
    temp = ((current + HEADER_SIZE) + size);
    temp->this.size = (current->this.size - size - HEADER_SIZE);
    temp->this.next = current->this.next;
    current->this.size = size;
    current->this.next = temp;
    */
}

Header *increase_heap(int size){
    Header *current;
    int amount, sizeBytes;
    char *ptr;


    /*
    if ((HEADER_SIZE + size) > 1024){
        amount = 1024;
    }else {
        amount = HEADER_SIZE + size;
    }
    */
    sizeBytes = size * HEADER_SIZE;
    amount = MIN(sizeBytes, 1024);


    //This line gets the addr for the current brake line
    ptr = sbrk(amount);

    //Test if sbrk returned error
    if (ptr == (char *) - 1){
        printf("SBRK ERROR!\n");
        return NULL;
    }
    current = (Header *)ptr;
    current->this.size = (ALIGN(amount) /16);
    //current->this.next = NULL;

    //extra->this.size = 1024 - amount;
    //if (extra->this.size > 0){
    free352((void *)(current+1));

    //Only execute if not first initialization
    //if (previous){
    //    previous->this.next = current;
    // }
    return freep;
}

void *malloc352(int nbytes){
    Header *current, *previous;
    int size;
    int listIndex;

    size = (ALIGN(nbytes) / 16) + 1;

    if ( (size >= 2) && (size <= 10) ){
        listIndex = size-1;
    }else {
        listIndex = 0;
        regular = 0;
    }
    freep = freeps[listIndex];

    //Check if base needs to be initialized
    if ((previous = freep) == NULL){
        //base = root;
        previous = &base;
        freep = previous;
        base.this.next = freep;
        base.this.size = 0;
    }


    //Verify that the list has been initialized
    if ((previous = freep) != NULL){
        previous = freep;
        current = previous->this.next;
         current = find_first_block(previous, current, size);

        if (current){
            //Check to see if we can break block up
            if ((current->this.size) >= size){
                if (current->this.size > (size)){
                    split(current, size);
                }
                else{
                    previous->this.next = current->this.next;
                }
                //previous->this.next = current->this.next;  //Used for allocating front end
                freep = previous;
            }
        }
    }

    #ifdef DEBUG
    totalBlocks += current->this.size;
    print_debug(freep);
    #endif
    return (void *)(current+1);

}

void *combineList(Header *new){
    new->this.next = freep->this.next;
    freep->this.next = new;
}

void *combine(Header *current, Header *new){
    Header *forward = (new + new->this.size);
    Header *previous = (current + current->this.size);

    if(current->this.next == new + new->this.size){
        new->this.size += current->this.next->this.size;
        new->this.next = current->this.next->this.next;
    }else{
        new->this.next = current->this.next;
    }

    if(current + current->this.size == new){
        current->this.size += new->this.size;
        current->this.next = new->this.next;
    }else{
        current->this.next = new;
    }
    freep = current;

}

Header *findSpot(Header *new){
    Header *current;

    for (current = freep; !(new > current && new < current->this.next); current = current->this.next){
        if (current >= current->this.next && (new > current || new < current->this.next)){
            break;
        }
    }
    return current;
}


void free352(void *ptr){


    Header *new = (Header *)ptr -1;
    #ifdef DEBUG
    totalFreed += new->this.size;
    #endif

    if (regular){
        combineList(new);
    }else{
        Header *spot = findSpot(new);
        combine(spot, new);
        freep = spot;
    }

    #ifdef DEBUG
    print_debug(freep);
    #endif
}

int main(){

    heap_test();

    return 0;
}
