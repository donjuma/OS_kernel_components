//
//  HeapTestEngine.c
//
//  program to test heap memory allocation
//
//  David Bover, April, 2015


#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

#include "heap352.h"

#define BUFF_SIZE   100

// test parameters
int reg_min;            // minimum size (bytes) of regular allocations
int reg_max;            // maximum size (bytes) of regular allocations
int large_max;          // maximum size (bytes) of all allocations
int reg_percent;        // percentage of allocations that are regular
int sample_size;        // total number of allocation requests

unsigned long *allocs;  // array of heap allocation addresses
int nallocs = 0;        // number of heap allocations in allocs

FILE *msglog;           // log file

/*
    function get_parameters()
    get the heap test parameters from the file Config.txt
*/
void get_parameters() {

    char line[BUFF_SIZE];
    FILE *config;

    if (!(config = fopen("Config.txt", "r"))) {
        printf("Cannot open configuration file Config.txt\n");
        exit(0);
    }

    fgets(line, BUFF_SIZE, config);
    sscanf(line, "%d", &reg_min);
    fgets(line, BUFF_SIZE, config);
    sscanf(line, "%d", &reg_max);
    fgets(line, BUFF_SIZE, config);
    sscanf(line, "%d", &large_max);
    fgets(line, BUFF_SIZE, config);
    sscanf(line, "%d", &reg_percent);
    fgets(line, BUFF_SIZE, config);
    sscanf(line, "%d", &sample_size);

}

/*  function get_size()
    generate a random size, based on configuration parameters
*/
int get_size () {

    if (random() % 100 > reg_percent)       // a large allocation
        return reg_max + random() % (large_max - reg_max);
    else                                    // a regular allocation
        return reg_min + random() % (reg_max - reg_min);
}

/*  function do_allocation()
    make a call to malloc352()
*/
void do_allocation (int sample) {
    void *address;
    address = malloc352(get_size());
    if (address == NULL)
        fprintf(msglog, "malloc352 fail on sample %d\n", sample);
    else {
        allocs[nallocs] = (unsigned long)address;
        nallocs++;
    }
}

/*  function do_free()
    make a call to free352() for a randomly-selected allocation
*/
void do_free () {
    int index;
    unsigned long address;

    if (nallocs == 0) return;

    index = random() % nallocs;
    address = allocs[index];
    allocs[index] = allocs[nallocs - 1];
    nallocs--;

    free352((void *)address);
}

/*  function heap_test()
    called to run the test engine
*/
void heap_test() {

    int sample;

    // seed the random number generator
    srand(time(NULL));

    // get the test parameters
    get_parameters();

    // start the log file
    msglog = fopen("log.txt", "w");

    // get the start address of the allocs array
    // allowing space for the array
    allocs = (unsigned long *) sbrk(sample_size * sizeof(unsigned long));

    // start the allocation requests
    for (sample = 0; sample < sample_size; sample++) {

        // start-up phase, sample < sample_size/10, all allocations
        if (sample < sample_size/10)
            do_allocation(sample);


        // build-up phase, sample < sample_size/2,
        // calls to free() in proportion to sample
        else if (sample < sample_size / 2) {
            if (random() % 100 > 200 * sample / sample_size)

                // an allocation
                do_allocation(sample);
            else
                // a de-allocation
                do_free();
        }

        // past half way through samples,
        // even balance between allocations and de-allocations
        else if (random() % 100 > 50)
            do_allocation(sample);
        else
            do_free();


    }
    // clean up remaining allocations
    while (nallocs > 0) do_free();
}
