#include "bsorts.h"

Sorts::Sorts()
{}

void Sorts::loadData(int data[], int len)
{
    fullLen = len;
    // Copy to generated data
    for(int i = 0; i < fullLen; i++)
        generatedData[i] = data[i];

    // Then run dataReset() to copy it into sorting array.
    dataReset();
}

sReturn Sorts::runSorts()
{
    // Run algs multiple times on data (DATARUNS times)

    // Do bubble sort runs
    for(int i = 0; i < SORT_DATA_RUNS; i++)
    {
        // Capture time
        clock_t t1 = clock();

        // Perform bubble sort
        bubblesort();

        //Capture time again
        clock_t t2 = clock();

        // Add run time to result array
        bubbleSortTimes[i] = timediff(t1, t2);

        // Reset operating data to originaly generated random numbers to ensure we are
        // comparing the sorting speed on the exact same random numbers
        dataReset();
    }

    // Repeat above process for other sorts

    // Do quick sort runs
    for(int i = 0; i < SORT_DATA_RUNS; i++)
    {
        clock_t t1 = clock();
        quickSort(array, 0, fullLen-1);
        clock_t t2 = clock();
        quickSortTimes[i] = timediff(t1, t2);
        dataReset();
    }

    // Do merge sort runs
    for(int i = 0; i < SORT_DATA_RUNS; i++)
    {
        clock_t t1 = clock();
        mergeSort(array, 0, fullLen-1);
        clock_t t2 = clock();
        mergeSortTimes[i] = timediff(t1, t2);

        dataReset();
    }

    // Build averages, and set total runtime stats
    float btime = 0.0, qtime = 0.0, mtime = 0.0;
    for(int i = 0; i < SORT_DATA_RUNS; i++)
    {
        btime += bubbleSortTimes[i];
        qtime += quickSortTimes[i];
        mtime += mergeSortTimes[i];
    }

    float totalTime = btime + qtime + mtime;
    btime = btime/SORT_DATA_RUNS;
    qtime = qtime/SORT_DATA_RUNS;
    mtime = mtime/SORT_DATA_RUNS;
    float totalAvg = totalTime/(SORT_DATA_RUNS*3);

    // Build return item and send it
    sReturn ret;
    ret.btime = btime;
    ret.qtime = qtime;
    ret.mtime = mtime;
    ret.ttime = totalTime;
    ret.tavgtime = totalAvg;
    return ret;
}

void Sorts::dataReset()
{
    for(int i = 0; i < SORT_DATA_SET; i++)
        array[i] = generatedData[i];
}

double Sorts::timediff(clock_t t1, clock_t t2)
{
    return (double)(t2 - t1) / CLOCKS_PER_SEC;
}

void Sorts::swap(int &a, int &b)
{
    int t = a;
    a = b;
    b = t;
}

void Sorts::bubblesort()
{
    int i, j;
    for(i=0;i<fullLen-1;i++)
        for(j=fullLen-1;j>i;--j)
            if(array[j] < array[j-1])
                swap(array[j],array[j-1]);
}

int Sorts::qpartition(int *arr, int left, int right)
{
    // Use mid as the pivot
    int p = arr[(left+right)/2];

    // Perform Hoare-Partitioning
    while ( left < right )
    {
        while ( arr[left] < p )
            left++;

        while ( arr[right] > p )
            right--;

        if ( arr[left] == arr[right] )
            left++;
        else if ( left < right )
            swap(arr[left], arr[right]);
    }
    return right;
}

void Sorts::quickSort(int *arr, int left, int right)
{
    if( left < right )
    {
        // Mid point
        int p = qpartition(arr, left, right);

        // Quicksort left of piv
        quickSort(arr, left, p-1);

        // Quicksort right of piv
        quickSort(arr, p+1, right);
    }
}

void Sorts::doMerge(int *arr, int left, int right)
{
    int p = (left + right) / 2,     // Mid point
            ti = 0,                 // Temp  indexer
            l = left,               // Left  indexer
            k = p + 1,              // Right indexer
            temp[right-left+1];     // Temp array

    // Merge the two data sets
    while ( l <= p && k <= right )
        if ( arr[l] < arr[k] )
            temp[ti++] = arr[l++];
        else
            temp[ti++] = arr[k++];

    // Merge remaining data in left arr
    while ( l <= p )
        temp[ti++] = arr[l++];

    // Merge remaining data in right arr
    while ( k <= right )
        temp[ti++] = arr[k++];

    // Copy temp array to master array
    for ( int i = left; i <= right; i++ )
        arr[i] = temp[i-left];
}

void Sorts::mergeSort(int * arr, int left, int right)
{
    if(left < right)
    {
        // Get mid point
        int p = (left + right) / 2;

        // Merge sort left side
        mergeSort(arr, left, p);

        // Merge sort right side
        mergeSort(arr, p + 1, right);

        // Perform the merging
        doMerge(arr, left, right);
    }
}
