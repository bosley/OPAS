#include <time.h>
#include <stdio.h>
#include <stdlib.h>

template <class T>
void swap ( T& a, T& b )
{
  T c(a);
  a = b;
  b = c;
}

template<class T>
void bubbleSort(T data[], int n)
{
    int i, j;
    for(i=0;i<n-1;i++)
        for(j=n-1;j>i;--j)
            if(data[j] < data[j-1])
                swap(data[j],data[j-1]);
}

template<class T>
void insertionSort(T data[], int n)
{
    int i,j;
    for (i=1;i<n;i++)
    {
        T tmp = data[i];
        for (j=i; j>0 && tmp<data[j-1]; j--){ data[j] = data[j-1]; }
        data[j] = tmp;
    }
}

template<class T>
void combSort(T data[], const int n)
{
    int step = n, j, k;
    while((step = int (step/1.3)) > 1)
    for(j=n-1;j>=step; j--)
    {
        k = j -step;
        if(data[j] < data[k])
            swap(data[j],data[k]);
    }
    bool again = true;
    for(int i = 0; i<n-1 && again; i++)
        for(j=n-1, again = false; j>1; --j)
            if(data[j] < data[j-1])
            {
                swap(data[j], data[j-1]);
                again = true;
            }
}

template<class T>
void reverseData(T x[], int n)
{
    T y[ n ];
    insertionSort(x, n);
    int i,j;
    j = n-1;
    for(i = 0; i<=n-1; i++)
    {
        y[j] = x[i];
        j--;
    }
    for(i = 0; i < n; i++)
    x[i] = y[i];
}

template<class T>
void genRandData (T data[], int n, int c)
{
    srand (time(NULL));
    for (int i=0;i<n;i++){ data[i] = rand() % c; }
}

int main()
{
    int i;
    int dataNum = 200;
    int data[ dataNum ];

    printf("\n\t Random Data \n");

    genRandData(data, dataNum, 50);

    i = 0;
    for ( i; i < dataNum; i++)
    {
        printf("[%i] -> %8i\n", i, data[i]);
    }

    printf("\n\t Sorted Random Data \n");

    combSort(data, dataNum);

    i = 0;
    for ( i; i < dataNum; i++)
    {
        printf("[%i] -> %8i\n", i, data[i]);
    }

    printf("\n\t Reversed Random Data \n");

    reverseData(data, dataNum);

    i = 0;
    for ( i; i < dataNum; i++)
    {
        printf("[%i] -> %8i\n", i, data[i]);
    }

    return 0;
}
