//
//  main.cpp
//  fib
//
//  Created by Josh Bosley on 9/17/15.
//  Copyright (c) 2015 Josh Bosley. All rights reserved.
//

/*
    Recursive fibonacci call
    ~ O( n^n )
 
    Linear fibonacci call
    O(n)
 */


#include <time.h>
#include <iostream>

// Calculated time difference
double timediff(clock_t t1, clock_t t2)
{
    return (double)(t2 - t1) / CLOCKS_PER_SEC;
}

// Recursive fibonacci function
long recFib(long n)
{
    if (n == 0 || n == 1)
        return 1;
    
    return recFib(n-1) * recFib(n - 2);
}

// Linear fibonacci function
void linFib(long n)
{
    long fib[n];
    fib[0] = fib[1] = 1;
    
    for( int i = 2; i < n; i++)
        fib[i] = fib[i-1] + fib[i-2];
}

int main()
{
    long n = 40;
    
    std::cout << " Calculating fib to : " << n << ". This could take some time." << std::endl;
    
    clock_t fib1s = clock();
    recFib(n);
    clock_t fib1e = clock();
    
    std::cout << "Recursive Fib : " << difftime(fib1e, fib1s) << std::endl;
    
    clock_t fib2s = clock();
    linFib(n);
    clock_t fib2e = clock();
    
    std::cout << "Linear Fib : " << difftime(fib2e, fib2s) << std::endl;
    
    return 0;
}
