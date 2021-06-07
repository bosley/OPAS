/*
The MIT License (MIT)

Copyright (c) 2016 Josh Bosley

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifndef BOHLL_H
#define BOHLL_H
#include <map>
#include <stack>
#include <vector>
#include <fstream>
#include <queue>
#include <iostream>
#include <sstream>
#include <iterator>

// Debug mode
#define BHLDEBUG            0
#define BHLCOMMENTCODE      1

// BASM comment generation mode
#define BHLCOMMENT          1

// Number of commands
#define COMMAND_COUNT       14

// Instructions
#define INSTRUCT_DEF        0
#define INSTRUCT_RET        1
#define INSTRUCT_REC        2
#define INSTRUCT_ARR        3
#define INSTRUCT_IF         4
#define INSTRUCT_FOR        5
#define INSTRUCT_FI         6
#define INSTRUCT_PUT        7
#define INSTRUCT_GO         8
#define INSTRUCT_DONE       9
#define INSTRUCT_FOUT       10
#define INSTRUCT_UINPT      11
#define INSTRUCT_EF         12
#define INSTRUCT_DO         13

// Var types
#define ARR                 0
#define DARR                1
#define VAR                 2

#define STR_VAL             0
#define INT_VAL             1
#define FLT_VAL             2

#define ITOS( x ) dynamic_cast< std::ostringstream & >( \
( std::ostringstream() << std::dec << x ) ).str()

#ifdef _WIN32
    #define LIBRARY_LOCATION "C:\\BLang\\Library\\"
#else
    #define LIBRARY_LOCATION "Library/"
#endif

// Index of item in variableList is the variable's id
// Example : variable[0] = m10, variable[4] = m(10+4)
struct variable
{
    std::string name;        // Name of the variable
    char dtype;              // Data type
    int ival;                // Int value
    std::string sval;        // String value
    float fval;              // Float value
    int mi;                  // Memory index
    unsigned x, y;           // For array variables
    char arrType;            // D for double, S for single
    std::string translation;
    std::string ftranslation;
    
   
    void operator= (const variable &n)
    {
        name    = n.name;
        dtype   = n.dtype;
        ival    = n.ival;
        sval    = n.sval;
        fval    = n.fval;
        mi      = n.mi;
        translation  = n.translation;
        ftranslation = n.ftranslation;
    }
};
typedef struct variable bvar;

// Storage and iterator for variables
typedef std::vector<bvar> bvec;
typedef std::vector<bvar>::iterator bvitr;

struct dataItems
{
    std::vector<char> ops;
    std::vector<std::string> items;
};
typedef struct dataItems ditems;

class Bohll
{
public:
    Bohll();
    
    // Completed
    void startFunction(std::string name);
    void endFunction();

    void callFunction(std::string name);
    
    void ifStatement(std::string statement);
    void endIfStatement();
    
    void print(std::string printExpression);
    
    void read(std::string var);
    
    void file(std::string statement);
    
    void label(std::string name);
    
    void forLoop(std::string statement);
    void endForLoop();
    
    void doDone();
    void doGo(std::string label);
    
    void createBlangFle();
    
    void newVariable(std::string variable);
    
    // Under Construction
    void newArray(bvar arr);
    void newDblArray(bvar dblarr);

    void doWhileLoop(std::string condition);
    void endDoWhileLoop();

    void whileLoop(std::string condition);
    void endWhileLoop();
    
    void recycleMemory(std::string line);
    
    void includeLibrary(std::string lib);                           // Add some functionality


private:
    
    // Indexers
    int csmi;
    int csmiTemp;
    
    // Flags
    bool printState;
    bool varAssignFlag;
    bool csmiControlFlag;
    
    // Counters
    unsigned ifStatementCounter;                                    // Counts the programs ifs
    unsigned labelBottomCounter;                                    // Countes the bottoms of functions
    unsigned functionCounter;                                       // Number of functions given
    unsigned forLoopCounter;                                        // Number of forloop
    
    // Variables
    char mops[7];                                                   // Math operators
    
    bvec variableList;                                              // Known variables
    
    std::vector<std::string> existingLables;                        // All known labels
    std::vector<std::string> existingFunctions;                     // All known functions
    std::vector<std::string> blangOutput;                           // The output file
    std::vector<std::string> includedLibs;                          // Libs currently used
    
    std::string currentFunctionBottomLabel;                         // Current function's corresponding bottom label
    std::string printStatementVariable;                             // Reserved name for temporary print variable
    
    std::stack<std::string> ifStatementLabels;                      // Labels for branching from if statements
    std::stack<std::string> forLoopEndCommands;                     // Bottom commands for a forloop
    std::queue<int> recycledMemory;                                 // Old spots to be re-used

    /*
     
            HELPER METHODS
     
     */
    std::vector<std::string> splitString(char c, std::string str);  // Split a string by a delimeter
    std::string stripChar(char c, std::string str);                 // Remove a evere instance of a char from as string
    std::string ltrim(std::string string, char del);                // Remove char from a string's lhs
    std::string rtrim(std::string string, char del);                // Remove char from a string's rhs
    std::string trim (std::string string, char del);                // Remove a char from a string's lhs, and rhs
    
    
    std::string postfix(std::string);                               // Convert a statement into postfix notation
    int analyzeDataType(std::string data);                          // Determine tha data type of a string
    void insertBasmComment(std::string comment);                    // Called for inserting comments into the basm file generated
    ditems explodeStatement(std::string statement);                 // Strips a statement into all of its data. tokens, types, and operators
    int getNexMemoryIndex();                                        // Retrieve a memory index for variable assignment
    
    unsigned retrieveMemoryIndexOfVariable(std::string variableName);   // Get the memory index of a variable
    
    /*
     
            CODE GENERATION METHODS
     
     */
    // Called by newVariable
    void constructString(std::string lhs, std::string rhs, bool hasOps);            // Create a new string!
    void constructNumber(std::string lhs, std::string rhs, bool hasOps);            // Create a new int!
    
    // Called by above code gen functions to create instruction set
    void setVariable(char type, std::string name, std::string value);               // Generate the code to create a variable
    
    // Called by setVariable when [] are found in var name. Will translate to the var name generated
    // by the array assigment procedure, and call setVariable with the data the inital setVariable call to this function passed it
    void setArrayVariable(char type, std::string name, std::string value, std::string x1, std::string y1);
    
    // Given the array name, and the xy items, get the memory index
    unsigned getArrayItemMemoryIndex(std::string name, std::string x1, std::string y1);
    
    
};

#endif // BOHLL_H
