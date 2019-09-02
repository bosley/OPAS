#include <iostream>
#include <fstream>

using namespace std;

class Sudoku
{
    public:
	Sudoku();	// Constructor
	~Sudoku();     	// Destructor
	int getElement(int x, int y);	// Accessor
	void setElement(int x, int y, int value); // Mutator
	bool solve();	// Recursive solution
	bool rowGood(int row, int guess); // Check row for guess violation
	bool colGood(int col, int guess); // Check col for guess violation
	bool squareGood(int row, int col,  int guess); // Check square for guess violation
	bool findOpenCell(int row, int col);
	void fillPuzzle(ifstream &infile);
	void printPuzzle();

    private:
	int puzzle[9][9];
};

