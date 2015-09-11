#ifndef SORTS_H
#define SORTS_H

#include <time.h>
#include <stdio.h>
#include <stdlib.h>

// For file output
#include <iostream>
#include <fstream>

// How many integers to generate
#define DATASET 100000

// How many times to run an algorithm on the data
#define DATARUNS 10

class Sorts
{
public:

    // Everything will happen here
    Sorts();

private:

    // Something to hold the random numbers
    int generatedData[DATASET];

    // A place for them to be sorted
    int array[DATASET];

    // Results of the algorithms
    double bubbleSortTimes[DATARUNS];
    double quickSortTimes[DATARUNS];
    double mergeSortTimes[DATARUNS];

    // Generate the initial data
    void generateData();

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

    // Display the results of the multiple runs
    void displayResults();
};

#endif // SORTS_H
