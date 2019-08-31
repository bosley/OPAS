#include <iostream>
#include <fstream>
#include "sudoku.h"
using namespace std;

Sudoku::Sudoku()
{
    int i, j;
    for (i = 0; i < 9; i++)
    {
	for (j = 0; j < 9; j++)
	    setElement(i,j,0);
    }
}

Sudoku::~Sudoku()
{
}

int Sudoku::getElement(int x, int y)
{
    if (x >= 0 && x < 9 && y >= 0 && y < 9)
	return(puzzle[x][y]);
    else
    {
	cout << "Sudoku::getElement - Requested array element does not exist";
	cout  << endl;
	return(-1);
    }
}

void Sudoku::setElement(int x, int y, int value)
{
    if (x >= 0 && x < 9 && y >= 0 && y < 9)
	puzzle[x][y] = value;
    else
    {
	cout << "Sudoku::getElement - Requested array element does not exist"; 
	cout  << endl;
    }
}

bool Sudoku::solve()
{
	int row, col;
	bool hasOpen = false;	
	for(row = 0; row < 9 && !hasOpen; row++)
		for(col = 0; col < 9 && !hasOpen; col++)
			if(findOpenCell(col, row))
				hasOpen = true;
	if(!hasOpen)
		return true;
	row--; col--;

	for(int digit = 1; digit <= 9; digit++)
	{
		if(rowGood(row, digit) && colGood(col, digit) && squareGood(row, col, digit))
		{
			setElement(col,row,digit);
			if(solve())
			{
				return true;
			}
			// Unset if we can't go further!
			puzzle[col][row] = 0;
		}
	}
	return false;
}

bool Sudoku::rowGood(int row, int guess)
{
	for (int k = 0; k < 9; k++)
		if (getElement(k,row) == guess)
			return false;
	return true;
}

bool Sudoku::colGood(int col, int guess)
{
	for(int i = 0; i < 9; i++)
		if (getElement(col,i) == guess)
			return false;
	return true;
}

bool Sudoku::squareGood(int row, int col, int guess)
{
	//set start locatio n for searching
	int sx = -1, sy = sx;
	     if(col <= 2) {sx = 0;}
	else if(col <= 5) {sx = 3;}
	else if(col <= 8) {sx = 6;}

	     if(row <= 2) {sy = 0;}
	else if(row <= 5) {sy = 3;}
	else if(row <= 8) {sy = 6;}

	if(sx == -1 || -1 == sy)
	{
		cout << "Error locating start xy" << endl;
		exit(0);
	}

	// Check the square
	for (int x = 0, cx = 0; x < 3; x++)
	{
		cx = sx + x;
		for(int y = 0, cy = 0; y < 3; y++)
		{
			cy = sy + y;
			if(getElement(cx,cy) == guess)
				return false;
		}
	}
	return true;
}

void Sudoku::fillPuzzle(ifstream &infile)
{
    int i,j,value;
    for (i = 0; i < 9; i++)
    {
	for (j = 0; j < 9; j++)
	{
	    infile >> value;
	    setElement(i,j,value);
	}
    }
	cout << "Loaded the puzzle : " << endl;
	printPuzzle();
}

void Sudoku::printPuzzle()
{
    int i, j;
    for (i = 0; i < 9; i++)
    {
	for (j = 0; j < 9; j++)
	    cout << getElement(i,j) << " ";
	cout << endl;
    }
}

bool Sudoku::findOpenCell(int row, int col)
{
	if (0 == getElement(row, col))
	{
		return true;
	}
	return false;
}

