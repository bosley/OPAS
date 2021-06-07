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

#include <stack>
#include <vector>
#include <sstream>
#include <fstream>
#include <iostream>

#include "frame/bohll.h"

#define MAX_RECURSE_THRESH 1000        // How many library inclusions allowed before considiring include loop

using namespace std;

// Check a string to check the existence of something
bool containsItem(std::string s, std::string item)
{
    bool inStr = false;
    std::string curr = "";
    for(unsigned i=0;i<s.length();i++)
    {
        if('"' == s[i])
            (inStr) ? inStr = false : inStr = true;
        curr += s[i];
        if(!inStr && item == curr)
            return true;
        curr = "";
    }
    return false;
}

// Trim the right side of the screen
std::string rtrim(std::string s, char del)
{
    size_t endpos = s.find_last_not_of(del);
    return (endpos == std::string::npos) ? "" : s.substr(0, endpos+1);
}

// Trim left side of the string
std::string ltrim(std::string s, char del)
{
    size_t startpos = s.find_first_not_of(del);
    return (startpos == std::string::npos) ? "" : s.substr(startpos);
}

// Calls left and right trim on the input
std::string trim(std::string s, char del)
{
    return rtrim(ltrim(s, del), del);
}

// Remove something from a string
std::string stripChar(char c, std::string str)
{
    std::string t = "";
    for(int i = 0; i < str.length(); i++)
    {
        if(str[i] != c)
            t += str[i];
    }
    return t;
}

// Check a string for ops
bool checkForLogicalOperators(std::string s)
{
    // Build a string without allowed substrings
    std::string check = "";
    bool inStr = false, didAd = false;
    for(unsigned i=0;i<s.length();i++)
    {
        if('"' == s[i])
            (inStr) ? inStr = false : inStr = true;
        if(!inStr && '"' != s[i])
        {
            check += s[i];
            didAd = false;
        }
        else if (inStr && !didAd && '"' != s[i])
        {
            check += " ";
            didAd = true;
        }
    }
    
    // Check for ops
    if(check.find("==") != std::string::npos)
        return true;
    if(check.find("||") != std::string::npos)
        return true;
    if(check.find("&&") != std::string::npos)
        return true;
    if(check.find("!=") != std::string::npos)
        return true;
    if(check.find("<")  != std::string::npos)
        return true;
    if(check.find(">")  != std::string::npos)
        return true;
    if(check.find("<=") != std::string::npos)
        return true;
    if(check.find(">=") != std::string::npos)
        return true;
    return false;
}

// Bring in the file!
// If we need to bring in a library, recurs this fucntion to bring it required files
void inputFile(const std::string arg, Bohll &bohll, int recursCounter)
{
    fstream input;
    input.open(arg);
    
    if(!input.is_open())
    {
        cout << "Can not find input file." << endl;
        exit(1);
    }
    
    //bvec existingVars;
    std::vector<std::string> existingLabels;                        // Ensure uniqueness
    std::vector<std::string> existingFunctions;
    
    // List of accepted 'tokens'
    std::string commands[COMMAND_COUNT] =
    {
        "def", "ret", "free", "arr", "if",
        "for", "fi" , "put", "go" , "done",
        "fout" , "uinput", "ef", "do"
    };
    
    int lineCount = 0;                                              // Line number in file for error reporting
    bool inFunc = false;                                            // For making sure that we don't have nested function
    std::string line, token;                                        // The input line, and the 'token' marking type of instruction
    while(std::getline(input, line))
    {
        token = "";
        lineCount++;
        line = trim(line, ' ');                                     // Clean-up the input line
        line = trim(line, '\n');
        line = trim(line, '\t');
        
        // Get rid of comments
        bool lis = false;
        std::string tLine = "";
        for(unsigned i = 0; i < line.length(); i++)                 // Detect, and remove comments from the line
        {
            if('"' == line[i])
            {
                (lis) ? lis = false : lis = true;
            }
            if(line[i] == '#' && !lis)
                i = line.length();
            else
                tLine += line[i];
        }
        line = tLine;
        
        if(line.length() != 0)                                      // Make sure we have an acutal line before operating on it
        {
            // Build the command token
            for(unsigned i = 0; i < line.length(); i++)                         // Grab the token
            {
                if(line[i] == ' ')
                    i = line.length();
                else
                    token += line[i];
            }
            
            token = stripChar(' ', token);
            
            /*
             Check for labels
             */
            if(line[0] == '@')                                       // Detect labels
            {
                for(unsigned i = 0; i < existingLabels.size(); ++i)  // Ensure not yet seen
                {
                    if(line == existingLabels[i])
                    {
                        std::cout << "Duplicate label : " << line
                        << ". On line " << lineCount << std::endl;
                        exit(1);
                    }
                }
                // Add the label to existing, and tell bohll to make it
                existingLabels.push_back(line);
                bohll.label(line);
                // Since we made the instruction, skip the rest
                continue;
            }
            
            /*
             Check for includes
             */
            if(line[0] == '[')
            {
                if(line[line.length()-1] != ']')
                {
                    std::cout << "Error : Library include cap ']' not found " << line << std::endl;
                    exit(1);
                }
                bohll.includeLibrary(line);
                continue;
            }
            
            /*
             Check for hl includes
             */
            if(line[0] == '{')
            {
                if(line[line.length()-1] != '}')
                {
                    std::cout << "Error : Library include cap '}' not found " << line << std::endl;
                    exit(1);
                }
                
                
                /*
                 
                        If we haven't hit the recurs threshold, recurse adding file
                 
                 */
                if(recursCounter < MAX_RECURSE_THRESH)
                    inputFile((LIBRARY_LOCATION + line), bohll, ++recursCounter);
                
                continue;
            }
            
            bool tFound = false;                                     // Make sure we found what we were looking for
            // Find the token
            for(unsigned i = 0; i < COMMAND_COUNT; i++)              // Check token against all known commands
            {
                if(token == commands[i])
                {
                    
                    tFound = true;
                    switch(i)                                        // Determine which action to take
                    {
                            /*
                             NEW FUNCTION
                             */
                        case INSTRUCT_DEF:
                        {
                            if(BHLDEBUG)
                                std::cout << "GOT INSTRUCTION DEF" << std::endl;
                            
                            if(inFunc)
                            {
                                std::cout << "Error: Nested function declaration on line : " << lineCount << std::endl;
                                exit(1);
                            }
                            inFunc = true;
                            
                            // Re-using token to get function name
                            token = "";
                            
                            // Make sure its an actual function name
                            if(line.length() <= 4)
                            {
                                std::cout << "Invalid function name : " << line
                                << ". On line : " << lineCount;
                                exit(1);
                            }
                            
                            for(unsigned j = 3; j < line.length(); j++)
                            {
                                if(' ' != line[j])
                                {
                                    token += line[j];
                                }
                            }
                            
                            if(checkForLogicalOperators(token))
                            {
                                std::cout << "Error: No operators are allowed in function definition\n";
                                exit(1);
                            }
                            
                            // Make sure that the function is unique
                            for(unsigned i = 0; i < existingFunctions.size(); ++i)
                            {
                                if(token == existingFunctions[i])
                                {
                                    std::cout << "Duplicate function name : " << token
                                    << ". On line : " << lineCount << std::endl;
                                    exit(1);
                                }
                            }
                            
                            // Add function name to existing functions
                            existingFunctions.push_back(token);
                            
                            // Create the function
                            bohll.startFunction(token);
                            break;
                        }
                            
                            /*
                             NEW FUNCTION RETURN
                             */
                        case INSTRUCT_RET:
                        {
                            if(BHLDEBUG)
                                std::cout << "GOT INSTRUCTION RETURN. LINE : " << lineCount << std::endl;
                            // Create end function label
                            // Set end func
                            
                            if(!inFunc)
                            {
                                std::cout << "Error: Item found outside of function on line : " << lineCount << std::endl;
                                exit(1);
                            }
                            inFunc = false;
                            
                            if(checkForLogicalOperators(token))
                            {
                                std::cout << "Error: No operators are allowed in ret statement\n";
                                exit(1);
                            }
                            
                            bohll.endFunction();
                            break;
                        }
                        case INSTRUCT_REC:
                        {
                            if(!inFunc)
                            {
                                std::cout << "Error: Item found outside of function on line : " << lineCount << std::endl;
                                exit(1);
                            }
                            
                            bohll.recycleMemory(line);
                            
                            
                            break;
                        }
                            
                            
                        case INSTRUCT_ARR:
                        {
                            if(!inFunc)
                            {
                                std::cout << "Error: Item found outside of function on line : " << lineCount << std::endl;
                                exit(1);
                            }
                            break;
                        }
                            
                            /*
                             NEW IF
                             */
                        case INSTRUCT_IF:
                        {
                            if(!inFunc)
                            {
                                std::cout << "Error: Item found outside of function on line : " << lineCount << std::endl;
                                exit(1);
                            }
                            if(!checkForLogicalOperators(line))
                            {
                                std::cout << "Error: No operators could be found in if definition on line : " << lineCount << std::endl;;
                                exit(1);
                            }
                            
                            bohll.ifStatement(line);                                    // create the if statement
                            
                            break;
                        }
                            
                            /*
                             NEW FOR LOOP
                             */
                        case INSTRUCT_FOR:
                        {
                            
                            if(!inFunc)
                            {
                                std::cout << "Error: Item found outside of function on line : " << lineCount << std::endl;
                                exit(1);
                            }
                            
                            bohll.forLoop(line);
                            break;
                        }
                            
                            /*
                             END FOR LOOP / IF
                             */
                        case INSTRUCT_FI:
                        {
                            if(!inFunc)
                            {
                                std::cout << "Error: Item found outside of function on line : " << lineCount << std::endl;
                                exit(1);
                            }
                            
                            bohll.endIfStatement();
                            break;
                            
                        }
                            
                            /*
                             NEW PUT INSTRUCTION
                             */
                        case INSTRUCT_PUT:
                        {
                            if(!inFunc)
                            {
                                std::cout << "Error: Item found outside of function on line : " << lineCount << std::endl;
                                exit(1);
                            }
                            // Complete
                            std::string v = "";
                            for(unsigned p=3; p < line.length();p++)
                            {
                                v+=line[p];
                            }
                            
                            if(checkForLogicalOperators(token) || containsItem(v, "="))
                            {
                                std::cout << "Error: Invalid 'put' syntax on line : " << lineCount << std::endl;;
                                exit(1);
                            }
                            
                            bohll.print(v);
                            break;
                        }
                            
                            /*
                             NEW GO INSTRUCTION
                             */
                        case INSTRUCT_GO:
                        {
                            
                            if(!inFunc)
                            {
                                std::cout << "Error: Item found outside of function on line : " << lineCount << std::endl;
                                exit(1);
                            }
                            bohll.doGo(line);
                            break;
                        }
                            
                            /*
                             NEW DONE INSTRUCTION
                             */
                        case INSTRUCT_DONE:
                        {
                            if(!inFunc)
                            {
                                std::cout << "Error: Item found outside of function on line : " << lineCount << std::endl;
                                exit(1);
                            }
                            bohll.doDone();
                            break;
                        }
                            
                            /*
                             NEW WRITE TO FILE
                             */
                        case INSTRUCT_FOUT:
                        {
                            if(!inFunc)
                            {
                                std::cout << "Error: Item found outside of function on line : " << lineCount << std::endl;
                                exit(1);
                            }
                            
                            if(checkForLogicalOperators(token))
                            {
                                std::cout << "Error: Invalid 'fout' syntax on line : " << lineCount << std::endl;;
                                exit(1);
                            }
                            bohll.file(line);
                            break;
                        }
                            
                            /*
                             NEW READ FROM USER
                             */
                        case INSTRUCT_UINPT:
                        {
                            
                            if(!inFunc)
                            {
                                std::cout << "Error: Item found outside of function on line : " << lineCount << std::endl;
                                exit(1);
                            }
                            bohll.read(line);
                            break;
                        }
                            /*
                             END FOR LOOP
                             */
                        case INSTRUCT_EF:
                        {
                            bohll.endForLoop();
                            break;
                        }
                            
                            /*
                             CALL A FUNCTION
                             */
                        case INSTRUCT_DO:
                        {
                            bohll.callFunction(line);
                            break;
                        }
                        default:
                        {
                            std::cout << "Error : Default accessed in token loop\n";
                            exit(1);
                            break;
                        }
                    }
                }
            }
            if(!tFound)                                              // If the item was not found above, check to see if its a var assignment
            {
                if(!containsItem(line, "=") || checkForLogicalOperators(line))
                {
                    std::cout << "Error : Invalid operation on line :" << lineCount << std::endl;
                    exit(1);
                }
                // If no token found, try to work it like a var
                bohll.newVariable(line);
            }
        }
    }
    input.close();
    bohll.createBlangFle();
}


// Entry point
int main(int argc, char *argv[])
{
    if(argc != 2)
    {
        cout << "\nNo input file give.\n";
        exit(1);
    }
    
    // Make a parser object
    Bohll bohll;
    
    std::string arg = argv[1];
    // Send bohll and file to input file. If a HL library is detectred, input file recurses the library
    // parsing the library code and adding it to the output file before parsing the current program
    // inputFile's recurse limit is set by MAX_RECURSE_THRESH
    inputFile(arg, bohll, 0);
    
    return 0;
}
