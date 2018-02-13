#ifndef DATASORTER_H
#define DATASORTER_H

#include <stdio.h>

namespace bosley
{

class DataSorter
{
public:
    DataSorter();
    ~DataSorter();

    template<class B>
    void sort(B *data);

private:

    // Line to cross before using advances search
    unsigned sizeThreshold;

    template<class B>
    void mergeData(B *arr, long long left, long long right);

    template<class B>
    void mergeSort(B *arr, long long left, long long right);

    template<class B>
    void swap(B &x, B &y);
};

DataSorter::DataSorter()
{
    sizeThreshold = 17;
}

DataSorter::~DataSorter()
{
    sizeThreshold = 0;
}

template<class B>
void DataSorter::swap(B &x, B &y)
{
    B t(x);
    x = y;
    y = t;
}

template<class B>
void DataSorter::mergeData(B *arr, long long left, long long right)
{
    long long p = (left + right) / 2,
            ti = 0,
            l = left,
            k = p + 1,
            temp[right-left+1];


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

template<class B>
void DataSorter::mergeSort(B *arr, long long left, long long right)
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
        mergeData(arr, left, right);
    }
}

template<class B>
void DataSorter::sort(B *data)
{
    // Get size of array
    long long size = sizeof(data)/sizeof(data[0]);

    // Simple sort, if its a small data set
    if(size <= sizeThreshold)
    {
        long long i, j;
        for(i=0;i<size-1;i++)
            for(j=size-1;j>i;--j)
                if(data[j] < data[j-1])
                    swap(data[j],data[j-1]);
    }
    else
    {
        // Merge sort if larger data set
        mergeSort(data, 0, size-1);
    }
}

}
#endif // DATASORTER_H
