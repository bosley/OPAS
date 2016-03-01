#ifndef BOSLEY_SORTS_H
#define BOSLEY_SORTS_H

#include <time.h>
#include <stdio.h>
#include <stdlib.h>

// For file output
#include <iostream>
#include <fstream>

// How many integers to generate
#define SORT_DATA_SET 100000

// How many times to run an algorithm on the data
#define SORT_DATA_RUNS 100

struct sortReturn
{
    float btime,
    qtime, mtime,
    ttime, tavgtime;
};
typedef struct sortReturn sReturn;

class Sorts
{
public:

    // Everything will happen here
    Sorts();
    void loadData(int data[], int len);
    sReturn runSorts();

private:

    // Something to hold the random numbers
    int generatedData[SORT_DATA_SET];

    // A place for them to be sorted
    int fullLen;
    int array[SORT_DATA_SET];

    // Results of the algorithms
    double bubbleSortTimes[SORT_DATA_RUNS];
    double quickSortTimes[SORT_DATA_RUNS];
    double mergeSortTimes[SORT_DATA_RUNS];

    // Reset data
    void dataReset();

    // Calculate time taken
    double timediff(clock_t t1, clock_t t2);

    // Perform swap action on two integers
    void swap(int& a, int& b);

    // Perform bubble sort
    void bubblesort();

    // Perform quick sort
    int qpartition(int * arr, int left, int right);
    void quickSort(int * arr, int left, int right);

    // Perform merge sort
    void doMerge(int * arr, int left, int right);
    void mergeSort(int * arr, int left, int right);
};

#endif // BOSLEY_SORTS_H
