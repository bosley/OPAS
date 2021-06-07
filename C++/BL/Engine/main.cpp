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
#include <locale>
#include <fstream>
#include <sstream>
#include <iostream>
#include <time.h>

#include "engine.h"

// Remove a char
std::string stripChar(char c, std::string str)
{
    std::string t = "";
    for(unsigned i = 0; i < str.length(); i++)
    {
        if(str[i] != c)
            t += str[i];
    }
    return t;
}

/*

        MEMORY LOADING FUNCTIONS

*/
// Check argument to see if its a register, mem slot, string, or int /long.
// Once function determines, it will send back a memslt struct with data so it
// won't have to be converted again
blang::mslt checkForRegister(std::string arg)
{
    if(BDEBUG)
        std::cout << "\tcheckForRegister(" << arg << ");" << std::endl;

    // Create a return item, and set type to n (null)
    blang::mslt retData;
    retData.type = 'n';

    // Checks a string if it is a register or not
    // r* -> Returns 'r'
    // m* -> Returns 'm'
    // $* -> Check if int or float
    //          - if int return 'i'
    //          - if float return 'f'

    bool hasPeriod = false,
         isString = false,
         isNumChain = true;

    std::string latterData = "";                // Data after r, m, $, or "

    if('"' == arg[0])
        isString = true;

    // Analyze the input, and extract latter data from reg/mem/raw/string call
    for(unsigned i = 1; i < arg.length(); i++)
    {

        // If its not a string
        if(!isString)
        {
            latterData += arg[i];

            if(arg[i] == '.')                   // Check to see if it has a period (Could be a long!)
            {
                hasPeriod = true;
            }
            else if(isNumChain)                 // Assuming true at first, if data is a numchain thus far
            {                                   // attempt to convert the item to an digit
                if(!std::isdigit(arg[i]))       // if any item can't be converted we can't convert to int or long
                {
                    isNumChain = false;
                }
            }
        }
        else
        {
            if(i < arg.length()-1 )
                latterData += arg[i];
        }
    }

    // Make sure data was valid
    if(!isString && !isNumChain)
    {
        std::cout << "Error: Invalid data in latter data of reg/mem/raw/string call" << std::endl;
        exit(1);
    }

    // Determine what to send back, and add to the return mslt
    switch(arg[0])
    {
    case 'r':                                               // Reg Slot
    {
        retData.type = 'r';
        std::stringstream ss(latterData);
        ss << latterData;
        ss >> retData.i;
        if(ss.fail())
        {
            std::cout << "Error: Non-int item found in register call" << std::endl;
            exit(1);
        }
        break;
    }
    case 'm':                                               // Mem Slot
    {
        retData.type = 'm';
        std::stringstream ss(latterData);
        ss << latterData;
        ss >> retData.i;
        if(ss.fail())
        {
            std::cout << "Error: Non-int item found in register call" << std::endl;
            exit(1);
        }
        break;
    }
    case '$':                                               // Raw value
    {
        std::stringstream ss(latterData);
        if(hasPeriod)                                       // Long
        {
            retData.type = 'f';
            ss << latterData;
            ss >> retData.f;
            if(ss.fail())
            {
                std::cout << "Error: Error converting to raw long" << std::endl;
                exit(1);
            }
        }
        else                                                // Int
        {
            retData.type = 'i';
            ss << latterData;
            ss >> retData.i;
            if(ss.fail())
            {
                std::cout << "Error: Error converting to raw integer" << std::endl;
                exit(1);
            }
        }
        break;
    }
    case '"':                                               // String
    {
        retData.type = 's';
        retData.s = (latterData);
        break;
    }
    default:
        std::cout << "Error: Invalid data found when checking for ins data" << std::endl;
        exit(1);
        break;
    }
    return retData;
}

/*
        INSERT ADD/SUB/MULT/DIV INSTRUCTIONS        - Complete
*/
void addInstruction_asmd(std::string ins, blang::eng &beng, unsigned cmdId)
{
    if(BDEBUG)
        std::cout << "ACCESSED: addInstruction_add(" << ins << ", beng, " << cmdId << ");" << std::endl;

    std::string temp = "";
    std::vector<std::string> members;
    for(unsigned i=0; i < ins.length(); ++i)
    {
        // Make sure no substrings are found inside the add command
        if('"' == ins[i])
        {
            std::cout << "Error: String item found in add command" << std::endl;
            exit(1);
        }
        // If a space is found, add temp into the members vec and reset
        // If its not a space, add it to temp
        if(' ' == ins[i])
        {
            members.push_back(temp);
            temp = "";
        }
        else
            temp += ins[i];
    }
    if(temp.length() >= 1 && temp[0] != ' ')
    {
        members.push_back(temp);
    }

    // Make sure nothing silly is happening
    if(members.size() > 3)
    {
        std::cout << "Error: Too many instructions within add command" << std::endl;
        exit(1);
    }

    // Get the information for what we are comparing
    blang::mslt s1 = checkForRegister(members[0]);
    blang::mslt s2 = checkForRegister(members[1]);
    blang::mslt s3 = checkForRegister(members[2]);

    // Send the data to be constructed
    blang::create_instruction(beng, cmdId, s1, s2, s3);

    if(BDEBUG)
        std::cout << "Add instruction inserted\n\n" << std::endl;
}

/*
        INSERT A JMP INSTRUCTION                    - Complete
*/
void addInstruction_jmp(std::string ins, blang::eng &beng, unsigned cmdId)
{
    // Can only jump to labels
    if(BDEBUG)
        std::cout << "ACCESSED: addInstruction_jmp(" << ins << ");" << std::endl;

    // Need to find location of label to jump to within the current frame's instruction set.
    // Ensure that mem dump works with jump, and that its goint

    // Clean up ins for locating pos
    std::string temp = "";
    for(unsigned i = 1; i <ins.length(); i++)
        temp += ins[i];

    // Locate the label address to jump to
    blang::mslt s1;
    s1.type = 'j';
    s1.s = temp;

    // Create the rest of the return items (blank) and create the instruction
    blang::mslt s2, s3;
    s2.type = 'i';
    s2.i = 0;
    s3.type = 'i';
    s3.i = 0;
    // Create the instruction
    blang::create_instruction(beng, cmdId, s1, s2, s3);
}

/*
        INSERT A CALL INSTRUCTION -                 - Complete
*/
void addInstruction_call(std::string ins, blang::eng &beng, std::vector<blang::mslt> &funcs, unsigned cmdId)
{
    if(BDEBUG)
        std::cout << "ACCESSED: addInstruction_call(" << ins << ");" << std::endl;

    // s1 stores index of function to call within engines fmem
    // s2 stores index of function that is doing the calling
    // s3 stores the instruction of the call within the current frame so we know where to start executing from
    //    once returned

    // Construct slot 1 (call)
    blang::mslt s1;
    s1.type = 'c';  // Set up as call
    for(unsigned i = 0; i < funcs.size(); ++i)
    {
        if(funcs[i].s == ins)
            s1.i = funcs[i].i;
    }

    blang::mslt s2;             // Call won't actually fill these out
    blang::mslt s3;             // but we make them to satisfy the params
    s2.type = 'c';
    s2.i = beng.cfc;            // The function that is doing the calling
    s3.type = 'c';
    s3.i = beng.fMem[beng.cfc].insSet.size(); // The address of this instruct within set to come back to
                                // as it hasn't been made yet. This does assume that there is at least
                                // one more instruction following the call
    // Create the instruction
    blang::create_instruction(beng, cmdId, s1, s2, s3);
}

/*
        INSERT A MOV INSTRUCTION                    - Complete
*/
void addInstruction_mov(std::string ins, blang::eng &beng, unsigned cmdId)
{
    if(BDEBUG)
        std::cout << "ACCESSED: addInstruction_mov(" << ins << ");" << std::endl;

    // Don't allow strings in destination slot
    if(ins[0] == '"')
    {
        std::cout << "Error: String value disallowed in mov destination" << std::endl;
        exit(1);
    }

    std::string temp = "";
    std::vector<std::string> members;

    ins = stripChar('\t', ins);
    ins = stripChar('\n', ins);
    bool inStr = false;
    for(unsigned i=0; i < ins.length(); ++i)
    {
        if('"' == ins[i])
            (inStr) ? inStr = false : inStr = true;

        // If a space is found, add temp into the members vec and reset
        // If its not a space, add it to temp
        if(' ' == ins[i] && !inStr)
        {
            members.push_back(temp);
            temp = "";
        }
        else
        {
            if('\t' != ins[i] && '\n' != ins[i])
                temp += ins[i];
        }
    }
    if(temp.length() >= 1 && temp[0] != ' ' && temp[0] != '\t' && temp[0] != '\n')
        members.push_back(temp);

    if(2 != members.size())
    {
        std::cout << "Error: Invalid number of instructions in mov command : " << members.size() << std::endl;
        for(unsigned s = 0; s < members.size(); s++)
            std::cout << "MEMBER [" << s << "]  =" << members[s] << std::endl;

        exit(1);
    }
    if('"' == members[0][0])
    {
        std::cout << "Error: String item found in mov destination" <<  std::endl;
        exit(1);
    }

    // Get reg data for branch
    blang::mslt s1 = checkForRegister(members[0]);
    blang::mslt s2 = checkForRegister(members[1]);
    blang::mslt s3;
    s3.type = 'i';      // s3 not used
    s3.i = 0;

    // Create the instruction
    blang::create_instruction(beng, cmdId, s1, s2, s3);
}

/*
        INSERT A BRANCH INSTRUCTION                 - Complete
*/
void addInstruction_branch(std::string ins, blang::eng &beng, unsigned cmdId, char which)
{
    /*
        INSTRUCTIONS RESULT

        slot1: lhs
        slot2: rhs
        slot3: i->label index to jump to, f->branch type
    */

    // Can only branch to labels, not functions
    if(BDEBUG)
        std::cout << "ACCESSED: addInstruction_branch(" << ins << ", beng, " << cmdId << ");" << std::endl;

    bool inStr = false;
    std::string temp = "";
    std::vector<std::string> members;
    for(unsigned i=0; i < ins.length(); ++i)
    {
        if('"' == ins[i])
            (inStr) ? inStr = false : inStr = true;
        
        // If a space is found, add temp into the members vec and reset
        // If its not a space, add it to temp
        if(' ' == ins[i] && !inStr)
        {
            members.push_back(temp);
            temp = "";
        }
        else
        {
            if('\t' != ins[i] && '\n' != ins[i])
                temp += ins[i];
        }
    }
    if(temp.length() >= 1 && temp[0] != ' ' && temp[0] != '\t' && temp[0] != '\n')
        members.push_back(temp);
    
    if(3 != members.size())
    {
        std::cout << "Error: Invalid number of instructions in branch command" << std::endl;
        exit(1);
    }

    // Get reg data for branch
    blang::mslt s1 = checkForRegister(members[0]);
    blang::mslt s2 = checkForRegister(members[1]);

    // 3 is label
    blang::mslt s3;
    s3.type = 'b';  // Set up as call

    // Remove @
    temp = "";
    for(unsigned i = 1; i < members[2].length(); i++)
        temp += members[2][i];

    // Store the item
    s3.s = temp;

    // Indicate which branch this is via the f member of mslt
    if(which == 'e')    // ble
        s3.f = 1.0;
    else if(which == 'n')    // bne
        s3.f = 2.0;
    else if(which == 'g')    // bgt
        s3.f = 3.0;
    else if(which == 'l')    // blt
        s3.f = 4.0;
    else if(which == 'q')
        s3.f = 5.0;
    else if(which == 'G')
        s3.f = 6.0;
    else
    {
        std::cout << "Error: Invalid branch command : " << which << std::endl;
        exit(1);
    }

    // Create the instruction
    blang::create_instruction(beng, cmdId, s1, s2, s3);
}

/*
        INSERT A PRINT INSTRUCTION                  - Complete
*/
void addInstruction_print(std::string ins, blang::eng &beng, unsigned cmdId)
{
    if(BDEBUG)
        std::cout << "ACCESSED: addInstruction_print(" << ins << ");" << std::endl;

    // Get slot information
    blang::mslt s1 = checkForRegister(ins);

    // Build the empty items that the create_instruction expects
    blang::mslt s2;
    blang::mslt s3;
    s2.type = 'i';          // S2, and S3 are not used by print yet
    s3.type = 'i';
    s2.i = 0;
    s3.i = 0;

    // Create the instruction
    blang::create_instruction(beng, cmdId, s1, s2, s3);
}

/*
        INSERT A READ INSTRUCTION                   - Complete
*/
void addInstruction_read(std::string ins, blang::eng &beng, unsigned cmdId)
{
    if(BDEBUG)
        std::cout << "ACCESSED: addInstruction_read(" << ins << ", bene, " << cmdId
                  << ")" << std::endl;

    blang::mslt s1 = checkForRegister(ins);

    // Build the empty items that the create_instruction expects
    blang::mslt s2;
    blang::mslt s3;
    s2.type = 'i';          // S2, and S3 are not used by print yet
    s3.type = 'i';
    s2.i = 0;
    s3.i = 0;


    // Create the instruction
    blang::create_instruction(beng, cmdId, s1, s2, s3);
}

/*
        INSERT A FILE INSTRUCTION                   - Complete
*/
void addInstruction_file(std::string ins, blang::eng &beng, unsigned cmdId)
{
    if(BDEBUG)
        std::cout << "ACCESSED addInstruction_file(" << ins << ", beng, " << cmdId << std::endl;

    bool inStr = false;
    std::string temp = "";
    std::vector<std::string> members;
    for(unsigned i=0; i < ins.length(); ++i)
    {
        if('"' == ins[i])
        {
            (inStr) ? inStr = false : inStr = true;
        }
        else
        {
            if(inStr)
            {
                temp += ins[i];
            }
            else if(' ' != ins[i])
            {
                temp += ins[i];
            }
            else if(' ' == ins[i])
            {
                members.push_back(temp);
                temp = "";
            }
        }
    }
    if(temp != "" && temp != " ")
        members.push_back(temp);

    if(members.size() != 3)
    {
        std::cout << "Error: Too many arguments in file instruction" << std::endl;
        exit(1);
    }

    // Need to do register checking information manually
    blang::mslt s1;
    blang::mslt s2;
    blang::mslt s3;

    temp = "";
    if(members[0][0] == 'r')
    {
        s1.type = 'r';
        for(unsigned i =1; i < members[0].length(); ++i)
            if(members[0][i] != ' ')
                temp+=members[0][i];
        std::stringstream ss(temp);
        ss << temp;
        ss >> s1.i;
        temp = "";
    }
    else if(members[0][0] == 'm')
    {
        s1.type = 'm';
        for(unsigned i =1; i < members[0].length(); ++i)
            if(members[0][i] != ' ')
                temp+=members[0][i];
        std::stringstream ss(temp);
        ss << temp;
        ss >> s1.i;
        temp = "";
    }
    else
    {
        std::cout << "Error : Invalid source in file instruction " << std::endl;
        exit(1);
    }

    if(members[1] == "a")
    {
        s2.type = 'F';
        s2.b = false;
    }
    else if( members[1] == "w")
    {
        s2.type = 'F';
        s2.b = true;
    }
    else
    {
        std::cout << "Error : Invalid instruction in file " << std::endl;
        exit(1);
    }

    temp = "";
    if(members[2][0] == 'r')
    {
        s3.type = 'r';
        for(unsigned i =1; i < members[2].length(); ++i)
            if(members[2][i] != ' ')
                temp+=members[2][i];
        std::stringstream ss(temp);
        ss << temp;
        ss >> s3.i;
        temp = "";
    }
    else if(members[2][0] == 'm')
    {
        s3.type = 'm';
        for(unsigned i =1; i < members[2].length(); ++i)
            if(members[2][i] != ' ')
                temp+=members[2][i];
        std::stringstream ss(temp);
        ss << temp;
        ss >> s3.i;
        temp = "";
    }
    else
    {
        std::cout << "Error : Invalid source in file instruction " << std::endl;
        exit(1);
    }

    blang::create_instruction(beng, cmdId, s1, s2, s3);
}

/*
        INSERT A LABEL INTO MEMORY FOR JUMPING TO   - Complete
*/
void addMemory_label(std::string ins, blang::eng &beng, unsigned cmdId, std::vector<blang::mslt> &labels)
{
    if(BDEBUG)
        std::cout << "ACCESSED: addMemory_label(" << ins << ");" << std::endl;

    // Insert instruction to indicate a label area
    // This instruction's location will be the area that is jumped to
    // Construct slot 1 (call)
    blang::mslt s1;
    s1.type = 'l';  // Set up as call
    for(unsigned i = 0; i < labels.size(); ++i)
    {
        if(labels[i].s == ins)
            s1.i = labels[i].i;
    }

    // Use the current size of the labels vector to derive a unique id
    unsigned labelId = labels.size();

    // Setup 2nd, and 3d slots
    blang::mslt s2;
    blang::mslt s3;
    s1.type = 'l';          // Indicate that the item is a label with 'l' . . .Clever eh?
    s1.i = labelId;         // Set the 'ID' of the label
    s2.type = 'l';          // Set s2, with the location of the instruction within insSet
    // s2.i set in create instruction
    s3.type = 'l';          // Set s3, but it will also remain unused
    s3.i = 0;

    // Build label info so we can locate it if we get a jmp or branch in
    blang::mslt labelInfo;
    labelInfo.s = ins;
    labelInfo.i = beng.insc+1;
    labels.push_back(labelInfo);

    // Create the instruction
    blang::create_instruction(beng, cmdId, s1, s2, s3);
}

/*
        INSERT A NEW FUNCTION                       - Complete
*/
void addMemory_function(std::string ins, blang::eng &beng, std::vector<blang::mslt> &funcs)
{
    if(BDEBUG)
        std::cout << "ACCESSED: addMemory_function(" << ins << ");" << std::endl;

    // Set up new frame
    blang::ffrm newFrame;                   // Create a blank function frame
    beng.fMem.push_back(newFrame);          // Add it to engine function memory
    beng.cfc = beng.fMem.size()-1;          // Use cfc to identify which frame we are building
    beng.fMem[beng.cfc].fMemLoc = beng.cfc;

    blang::mslt newFunc;                    // New func mslt data so we can translate name to mem index later
    newFunc.s = ins;                        // Name of func
    newFunc.i = beng.cfc;                   // Location
    funcs.push_back(newFunc);               // Add the data into the function vector

    if(BDEBUG)
        std::cout << "\tINSERTED: New engine function frame (" << beng.cfc << ")" << std::endl;
}

/*  -   -   -   -   -   -   -   -   - INPUT HANDLING    -   -   -   -   -   -   -   -   -   */

// Trim the right side of the screen
std::string inline rtrim(std::string s)
{
        size_t endpos = s.find_last_not_of(' ');
        return (endpos == std::string::npos) ? "" : s.substr(0, endpos+1);
}

// Trim left side of the string
std::string inline ltrim(std::string s)
{
        size_t startpos = s.find_first_not_of(' ');
        return (startpos == std::string::npos) ? "" : s.substr(startpos);
}

// Calls left and right trim on the input
std::string inline trim(std::string s)
{
        return rtrim(ltrim(s));
}

/*
        CLEARN UP A STRING
*/
std::string sanitizeString(std::string str)
{
    std::string ret = "";
    bool inAllowedSubStr = false;           // Indicate if scanner is inside an allowed substring
    for(unsigned i = 0; i < str.length(); i++)
    {
        // See char indicates allowed substring (something that can keep its data verbatim)
        // Then check the current state of being within the substr. If we aren't we've entered. If we are, we've exited
        if(str[i] == '"')
            (inAllowedSubStr) ? inAllowedSubStr = false : inAllowedSubStr = true;

        // If scanner within the allowed substr, add the charachter no matter what
        // If scaner is not, then prune away the unwanteds.
        if(inAllowedSubStr)
        {
            ret += str[i];
        }
        else
        {
            // If its a comment, stop taking this line seriously
            if('/' == str[i])
                break;
            if( ('\t' != str[i]) && ('\n' != str[i]))
                ret += str[i];
        }
    }
    return trim(ret);
}

/*
        ENTRY POINT

            Read in each line, and determine type of instruction
            Remove first intsruction, and send the data to the corresponding function
            In each function, perform error checking on syntax. While error checking, determine what to
                the operation of instruction.
*/
int main(int argc, char *argv[])
{
    // Start time
    clock_t time_start, time_parse, time_process;
    if(DISPLAY_RUN_TIME)
    {
        time_start = clock();
    }

    if(argc <= 1)
    {
        std::cout << "No input file given." << std::endl;
        return 1;
    }

    std::ifstream ifile;
    ifile.open(argv[1]);
    if(!ifile.is_open())
    {
        std::cout << "Could not read file : " << argv[1] << std::endl;
        return 1;
    }

    if(BDEBUG)
        std::cout << "\n\n\n\t--- INITIATE ---\n\n" << std::endl;

    // The B Engine
    blang::eng beng;
    beng.cfc = 0;
    beng.insc = 0;

    // To handle ensuring unique functions and labels
    std::vector<std::string> existingLabels;
    std::vector<std::string> existingFunctions;

    // For converting names to mem locations
    std::vector<blang::mslt> labelTranslations;             // Sent to mem creation functions to translate
    std::vector<blang::mslt> functionTranslations;          // called functions from strings to addresses - - - --  NOT YET DONE
    std::string line ,temp,
                currFunc,
                command, instructions;

    // List of all commands
    std::string valid_primary[PRIMARY_COMMANDS] = {"mult", "div", "add", "sub", "jmp",
                                    "call", "mov", "ble", "bne", "bgt",
                                    "blt" , "p", "r", "f", "beq", "bge"};
    int lineCount = 1;                  // Number of the line that we are on
    unsigned entryPoint;
    bool cmdFlag = false;               // Determine if scanner is reading cmd or inst
    bool buildingFunction = false;      // Flag to indicate the building of a function
    bool foundEntryPoint = false;
    bool valid = false;
    while(std::getline(ifile, line))
    {
        // Before we can do anything with the line, we need to clean it
        line = sanitizeString(line);

        if(line.length() >= 1 && line != " ")
        {
            /*
                    CHECK FOR FUNCTION / LABEL CREATION
            */
            valid = false;
            switch(line[0])
            {

            /*
                    The start of a new function was found
            */
            case '<':
                valid = true;
                if(BDEBUG)
                    std::cout << "Start of a func" << std::endl;
                // Disallow the nesting of function
                if(buildingFunction)
                {
                    std::cout << "Error: Can not have nested functions. [Line: "
                              << lineCount << " ]" << std::endl;
                    return 1;
                }
                temp = "";
                for(unsigned i = 1; i < line.length(); i++)
                {
                    if(' ' != line[i])
                        temp += line[i];                // Remove < and send func name to 'newfunc;
                }
                // Make sure the function is unique
                for(unsigned i = 0; i < existingFunctions.size(); ++i)
                {
                    if(temp == existingFunctions[i])
                    {
                        std::cout << "Error: Duplicate function [" << temp << "] on [Line: " << lineCount << std::endl;
                        return 1;
                    }
                }
                buildingFunction = true;                    // Mark us as buiding a function.
                currFunc = temp;                            // Set the current function
                temp = "";                                  // Reset temp
                addMemory_function(currFunc, beng, functionTranslations);  // Add the function

                // If entry point hasn't been found yet, and this function is the entry point,
                // indidcate found and set the id for calling
                if(!foundEntryPoint && "main" == currFunc)
                {
                       foundEntryPoint = true;
                       entryPoint = beng.cfc;   // Current function counter

                }
                break;

            /*
                The end of a function was found
            */
            case '>':
            {
                if(BDEBUG)
                    std::cout << "End of a func" << std::endl;
                // Ensure that we are anticipating a >
                if(!buildingFunction)
                {
                    std::cout << "Error: Attempting to end no-existing function [Line: "
                              << lineCount << " ]" << std::endl;
                    return 1;
                }
                valid = true;

                /*
                            Go back through instructions. anything that is a jmp or a branch,
                            find the matching label index and set it
                 */
                for(unsigned i = 0; i < beng.fMem[beng.cfc].insSet.size(); ++i)
                {
                    // If its a branch instruction, find the label it calls
                    if(INSTRUCT_BGT == beng.fMem[beng.cfc].insSet[i].operation ||
                            INSTRUCT_BLE == beng.fMem[beng.cfc].insSet[i].operation ||
                            INSTRUCT_BLT == beng.fMem[beng.cfc].insSet[i].operation ||
                            INSTRUCT_BNE == beng.fMem[beng.cfc].insSet[i].operation ||
                            INSTRUCT_BEQ == beng.fMem[beng.cfc].insSet[i].operation ||
                            INSTRUCT_BGE == beng.fMem[beng.cfc].insSet[i].operation)
                    {
                        bool found = false;
                        // Look for the translation
                        for(unsigned j = 0; j < labelTranslations.size(); ++j)
                        {
                            if(labelTranslations[j].s == beng.fMem[beng.cfc].insSet[i].addrSlot3.s)
                            {
                                found = true;
                                beng.fMem[beng.cfc].insSet[i].addrSlot3.i = labelTranslations[j].i;
                            }
                        }
                        if(!found)
                        {
                            std::cout << "Error : Unable to find branch location on line : " << lineCount << std::endl;
                            exit(1);
                        }
                    }

                    // If its a jump instruction, find the label it calls
                    if(INSTRUCT_JMP == beng.fMem[beng.cfc].insSet[i].operation)
                    {
                        bool found = false;
                        // Look for the translation
                        for(unsigned j = 0; j < labelTranslations.size(); ++j)
                        {
                            if(labelTranslations[j].s == beng.fMem[beng.cfc].insSet[i].addrSlot1.s)
                            {
                                found = true;
                                beng.fMem[beng.cfc].insSet[i].addrSlot1.i = labelTranslations[j].i;
                            }
                        }
                        if(!found)
                        {
                            std::cout << "Error : Unable to find jmp location on line : " << lineCount << std::endl;
                            exit(1);
                        }
                    }

                }
                existingFunctions.push_back(currFunc);  // Add the name to existing functions now that it is created
                buildingFunction = false;               // Signal that we are no longer in a function
                labelTranslations.clear();              // Delete the labels used in the current function.
                                                        // Labels are a strictly local thing
                break;
            }

            /*
                A label was found
            */
            case '@':
                if(BDEBUG)
                    std::cout << "Start of a label" << std::endl;
                valid = true;
                temp = "";                              // Make sure no weird data comes up
                for(unsigned i = 1; i < line.length(); i++)
                    temp += line[i];                    // Remove @ and send label name to mem func;
                // Make sure the label is unique
                for(unsigned i = 0; i < existingLabels.size(); ++i)
                {
                    if(temp == existingLabels[i])
                    {
                        std::cout << "Error: Duplicate label [" << temp << "] on [Line: " << lineCount << std::endl;
                        return 1;
                    }
                }
                addMemory_label(temp, beng, INSTRUCT_LABEL, labelTranslations);                  // Add the label
                existingLabels.push_back(temp);         // Add the name to existing labels
                temp = "";                              // Reset temp
                break;
            }

            /*
                    CHECK FOR COMMANDS
            */

            // Seperate initial command from instructions
            command = "";
            cmdFlag = false;
            instructions = "";
            for(unsigned i = 0; i < line.length(); ++i)
            {
                if(!cmdFlag &&' ' == line[i])
                    cmdFlag = true;
                else if(cmdFlag)
                    instructions += line[i];
                else if(!cmdFlag)
                    command += line[i];
            }

            // Check command against list of commands
            for(unsigned i = 0; i < PRIMARY_COMMANDS; ++i)
            {
                if(command == valid_primary[i])
                {
                    valid = true;
                    if(i < 4)
                    {
                        addInstruction_asmd(instructions, beng, i);
                    }
                    else if(i == 4)
                    {
                        addInstruction_jmp(instructions, beng, i);
                    }
                    else if(i == 5)
                    {
                        addInstruction_call(instructions, beng, functionTranslations, i);
                    }
                    else if(i == 6)
                    {
                        addInstruction_mov(instructions, beng, i);
                    }
                    else if(i >= 7 && i < 11)
                    {
                        // 7=ble,8=bne,9=bgt,10=blt
                        if(7==i)
                            addInstruction_branch(instructions, beng, i, 'e');
                        if(8==i)
                            addInstruction_branch(instructions, beng, i, 'n');
                        if(9==i)
                            addInstruction_branch(instructions, beng, i, 'g');
                        if(10==i)
                            addInstruction_branch(instructions, beng, i, 'l');
                    }
                    else if (i == 11)
                    {
                        addInstruction_print(instructions, beng, i);
                    }
                    else if(i == 12)
                    {
                        addInstruction_read(instructions, beng, i);
                    }
                    else if(i == 13)
                    {
                        addInstruction_file(instructions, beng, i);
                    }
                    else if(i == 14)
                    {
                        addInstruction_branch(instructions, beng, i, 'q');
                    }
                    else if(i == 15)
                    {
                        addInstruction_branch(instructions, beng, i, 'G');
                    }
                    else
                    {
                        std::cout << "Error: Unknown instruction ID : " << i << std::endl;
                        return 1;
                    } // End if chain
                } // End if
            } // End for

            if(!valid)
            {
                std::cout << "Error : Invalid instruction on line : " << lineCount << std::endl;
                return 1;
            }

        } // End outer-most if
        ++lineCount;
    } // End while
    // Close the file
    ifile.close();

    /*
            AT THIS POINT WE CLEAN UP ALL DATA NOT BEING USED BY THE ENGINE.
            CLEAR ALL VECTORS. DESTROY ALL HUMANS
            ALSO: RESET USED INDEXERS IN ENGINE USED IN MEM LOAD
    */
    existingLabels.clear();         // Clean out the vectors to ensure uniqueness and derive
    labelTranslations.clear();      // ids for building within engine mem.
    existingFunctions.clear();
    functionTranslations.clear();

    if(!foundEntryPoint)
    {
        std::cout << "No function 'main' found. Halting" << std::endl;
        exit(1);
    }

    // If enabled, dump out the memory so we can do error checking
    if(MEM_DUMP_BEFORE_PROCESS)
        blang::memory_dump(beng);

    if(BDEBUG)
        std::cout << "\n\n - - - - - - - [ABOUT TO PROCESS] - - - - - - - -\n" << std::endl;

    // Parsing done, get the time
    if(DISPLAY_RUN_TIME)
        time_parse = clock();

    // Start running the engine
    blang::process(beng, entryPoint);

    // Process done, get the time
    if(DISPLAY_RUN_TIME)
    {
        time_process = clock();
        std::cout << "[ - - - - - - - - - - - - - - - - - - - - - - - - ]"
                  << "\n\n\t\t COMPLETE \n\n";
        printf("\tParse + Load Time\t: %.8fs\n", (double)(time_parse - time_start)/CLOCKS_PER_SEC);
        printf("\tExecution Time\t\t: %.8fs\n\n", (double)(time_process - time_start)/CLOCKS_PER_SEC);
        std::cout << "[ - - - - - - - - - - - - - - - - - - - - - - - - ]" << std::endl;
    }

    return 0;
}
