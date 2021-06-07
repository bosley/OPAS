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

#include "bohll.h"

Bohll::Bohll()
{
    csmi = 0;                                                           // Current shared memory index
    printState = false;                                                 // Indicate if we are working a print statement
    varAssignFlag = false;                                              // Indicates
    csmiControlFlag = false;
    strcpy(mops,"+-*/^%");                                              // Math operations
    functionCounter = 0;
    labelBottomCounter = 0;                                             // Init func bottom counter
    ifStatementCounter = 0;                                             // Init if counter
    forLoopCounter = 0;
    currentFunctionBottomLabel = "";                                    // Init function bottom indicator
    printStatementVariable = "___bohlang_pvar___";                      // Reserved Print variable
}

/*
 ###############################################
 
                PUBLIC SECTION
 
 ###############################################
 */

/*
        CREATE A FUNCTION   - COMPLETE
*/
void Bohll::startFunction(std::string name)
{
    if(BHLDEBUG)
        std::cout << "NEW FUNC -> " << name << std::endl;
    
    name = trim(name, ' ');
    name = trim(name, '\t');
    name = trim(name, '\n');
    
    for(unsigned i = 0; i < existingFunctions.size(); i++)
    {
        if(name == existingFunctions[i])
        {
            std::cout << "Error : Duplicate function name : " << name << std::endl;
            exit(1);
        }
    }
    existingFunctions.push_back(name);                                  // Add to list of exisitng
    blangOutput.push_back(("<"+name));                                  // Add it to the file\
                                                                        // Create a label for early returns from the function
    currentFunctionBottomLabel = ("@__" + name + "_FUNCTION_BOTTOM_" + ITOS(labelBottomCounter));
    functionCounter++;
}

/*
        END A FUNCTION      - COMPLETE
*/
void Bohll::endFunction()
{
    if(BHLDEBUG)
        std::cout << "END FUNC " << std::endl;
    
    if(BHLCOMMENTCODE)
        insertBasmComment("Create function-end label for early return");
    
    label(currentFunctionBottomLabel);                                  // Add the 'bottom' label prior to end in case of early exit
    blangOutput.push_back(">");                                         // End the function
    labelBottomCounter++;                                               // Inc the bottom counter
}

/*
        CALL A FUNCTION
 */
void Bohll::callFunction(std::string statement)
{
    bool flag = false;
    std::string temp = "";
    for(unsigned i = 0; i < statement.length(); i++)
    {
        if(statement[i] == ' ')
            flag = true;
        
        if (flag)
            temp += statement[i];
    }
    
    if(temp != "")
        statement = temp;
    else
    {
        std::cout << "Error: No function given in call statement : " << statement << std::endl;
        exit(1);
    }
    
    statement = trim(statement, ' ');
    statement = trim(statement, '\t');
    
    flag = false;
    for(unsigned i = 0; i < existingFunctions.size(); i++)
    {
        if(existingFunctions[i] == statement)
            flag = true;
    }
    if(!flag)
    {
        std::cout << "Error: Call to non-existing function : " << statement << std::endl;
        exit(1);
    }
    
    blangOutput.push_back("\tcall " +  statement);
    
}

/*
        CREATE NEW VARIABLE
*/
void Bohll::newVariable(std::string input)
{
    if(BHLDEBUG)
        std::cout << "NEW VAR -> " << input << std::endl;

    // Get data from input, and clean it up
    std::vector<std::string> mySides( splitString('=', input) );
    std::string rhs = stripChar('\n', mySides[1]);
    std::string lhs = stripChar('\n', mySides[0]);
    lhs = stripChar(' ', lhs);
    lhs = trim(lhs, ' ');
    lhs = trim(lhs, '\t');
    lhs = trim(lhs, '\n');
    rhs = trim(rhs, ' ');
    
    // Gross re-use, but whatever.
    // Its a safety thing.
    std::string commands[COMMAND_COUNT] =
    {
        "def", "ret", "free", "arr", "if",
        "for", "fi" , "put", "go" , "done",
        "fout" , "uinput", "ef", "do"
    };
    for(unsigned i = 0; i < COMMAND_COUNT; i++)
    {
        if(lhs == commands[i])
        {
            std::cout << "Error: Can not assign newVariable with reserved word " << commands[i] << std::endl;
            exit(1);
        }
    }
    
    bool inStr = false;
    for(unsigned i = 0; i < lhs.length(); i++)
    {
        for(unsigned j =0; j < 6; j++)
        {
            if(lhs[i] == mops[j])
            {
                std::cout << "Error: Illegal char : " << mops[j] << ". found in variable name." << std::endl;
                exit(1);
            }
            if(lhs[i] == '"' || lhs[i] == '\\' || lhs[i] == '$' ||lhs[i] == '/' || lhs[i] == '%' || lhs[i] == '@'
               || lhs[i] == '<' || lhs[i] == '>')
            {
                std::cout << "Error: Illegal char found in variable name." << std::endl;
                exit(1);
            }
        }
    }
  
    /*
     
                CHECK TO SEE IF THE VARIABLE IS AN ARRAY
     
     */
    
    
    if(rhs[0] == '*')
    {
        bool doubleArr = false;
        rhs = stripChar(' ', rhs);
        rhs = trim(rhs, '\t');
        rhs = trim(rhs, '\n');
        
        std::string sizeTokenX, sizeTokenY;
        for(unsigned i = 1; i < rhs.length(); i++)
        {
            if(rhs[i] == ',')
            {
                doubleArr = true;
                continue;
            }
            
            if(!doubleArr)
            {
                sizeTokenX += rhs[i];
            }
            else
            {
                sizeTokenY += rhs[i];
            }
        }
        
        // Convert the given x, and y to an unsigned type
        unsigned arrX = 0, arrY = 0;

        std::stringstream ss;
        ss << sizeTokenX;
        ss >> arrX;
            
        if(ss.fail())
        {
            std::cout << "Error: Invalid array length given for X value!" << std::endl;
            exit(1);
        }
        if(doubleArr)
        {
            std::stringstream ss;
            ss << sizeTokenY;
            ss >> arrY;
            
            if(ss.fail())
            {
                std::cout << "Error: Invalid array length given for Y value!" << std::endl;
                exit(1);
            }
        }

        // Now that the X, and Y are given, need to create initial variable (var[0])
        
        /*
         
                Create single var var[0]
         
                var = * 10, 7
                var[2][5] location in linear memory is = 10 * 2 + 5 + 1 away from var[0] - Tested
         
                New array variable's init point is marked by a dtype of 'A'

         */
        
        // Create the initial variable for array ie var[0][0]
        newVariable(lhs + " = \"__bohl_array_init_point__\"");
        
        // Find the var we just made
        unsigned arrVarIndex;
        bool arrFound = false;
        for(unsigned i = 0; i < variableList.size(); i++)
        {
            if(variableList[i].name == lhs)
            {
                arrFound = true;
                arrVarIndex = i;
                variableList[i].x = arrX-1;       // Set the X, Y of this array
                variableList[i].y = arrY-1;
                
                if(doubleArr)
                     variableList[i].arrType = 'D';
                else
                     variableList[i].arrType = 'S';
            }
        }
        
        // Create the array in memory
        std::string lhsNewTrans;
        insertBasmComment("[START OF ARRAY MEMBER ASSIGNMENT]");
        if(!doubleArr)
        {
            // Creat the X-axis variables
            for(unsigned i = 0; i < arrX; i++)
            {
                // Create a space in memory for the array object
                lhsNewTrans = ("__bohl_arrX_var__" + lhs + "_" + ITOS(i) + " = 0");
                newVariable(lhsNewTrans);
                
                // Find the new var
                for(unsigned q = 0; q < variableList.size(); q++)
                {
                    if(variableList[q].name == lhsNewTrans)
                    {
                        variableList[q].arrType = 'A';          // Mark that its an array item
                    }
                }
            }
        }
        else
        {
            // Create Y-axis variables
            for(unsigned i = 0; i < arrY; i++)
            {
                // Create X-axis variables
                for(unsigned j = 0; j < arrX; j++)
                {
                    //
                    lhsNewTrans = ("__bohl_arrY_" + ITOS(i) + "_var__" + lhs + "_arrX_" + ITOS(j) + " = 0");
                    newVariable(lhsNewTrans);
                    
                    // Find the new var
                    for(unsigned q = 0; q < variableList.size(); q++)
                    {
                        if(variableList[q].name == lhsNewTrans)
                        {
                            variableList[q].arrType = 'A';          // Mark that its an array item
                        }
                    }
                }
            }
        }
        insertBasmComment("[END OF ARRAY MEMBER ASSIGNMENT]");
        return;
    }
    
    /*
     
            IF NEW VAR IS NOT AN ARRAY, CONTINUE ASSIGMENT
     
     */
    
    if(BHLCOMMENTCODE)
        insertBasmComment(("Create variable [" + lhs + "]"));
    
    // We need to know if we are dealing with any strings or not
    bool isString = false;
    for(int i = 0; i < rhs.length(); i++)
        if(rhs[i] == '"')
            isString = true;
    /*
     At this point, we want to go through the rhs
     and tell if there are any operations happening.
     We need to make sure that we don't mistake an
     op that is part of the string for an actual op
     */
    bool inString = false, hasOps = false;
    for(int i = 0; (i < rhs.length() && !hasOps); i++)
    {
        if(isString)
        {
            if(rhs[i] == '"')                                           // Toggle if we are scanning within "s
                (!inString) ? inString = true : inString = false;
            else
                if(!inString)                                           // If we aren't in "s, scan for ops
                    for(int j = 0; j  < 6 ; j++)
                        if(rhs[i] == mops[j])
                            hasOps = true;
        }
        else
        {
            for(int j = 0; j  < 6 ; j++)                                // If the data isn't a string
                if(rhs[i] == mops[j])                                   // We need to test each item
                    hasOps = true;
        }
    }
    
    if(lhs == printStatementVariable)                                   // Make sure its not a print variable name! Woohoo
    {
        if(!printState)
        {
            std::cout << "Error : Can not use reserved print variable : " << printStatementVariable << std::endl;
            exit(1);
        }
    }
    
    if(isString)
    {
        constructString(lhs, rhs, hasOps);                              // Create a new string variable
    }
    else
    {
        if(STR_VAL == analyzeDataType(rhs))
            constructString(lhs, rhs, hasOps);
        else
            constructNumber(lhs, rhs, hasOps);
    }
}

void Bohll::newArray(bvar var)
{
    if(BHLDEBUG)
        std::cout << "NEW ARR  "<< std::endl;
}

void Bohll::newDblArray(bvar var)
{
    if(BHLDEBUG)
        std::cout << "NEW DBL ARR  "<< std::endl;

}

/*
        CREATE FOR LOOP
*/
void Bohll::forLoop(std::string statement)
{
    if(BHLDEBUG)
        std::cout << "NEW FORLOOP -> " << statement << std::endl;
    
    unsigned flag = 0;
    std::string var = "", start = "", end = "", step = "na";        // Obtain the loop info
    for(unsigned i = 3; i < statement.length(); i++)
    {
        if(statement[i] != ' ')
        {
            if(1 == flag)
            {
                var += statement[i];
            }
            else if (3 == flag)
            {
                start += statement[i];
            }
            else if (5 == flag)
            {
                end += statement[i];
            }
            else if (7 == flag)
            {
                if(step == "na")
                    step = "";
                step += statement[i];
            }
        }
        else
        {
            flag++;
        }
    }
    
    // Test the pieces of the loop for error checking, and seeing if the items are variables or not
    int test;
    
    bool startIsVar = false,
            endIsVar = false,
            stepIsVar = false;
    
    std::stringstream ss0;
    std::stringstream ss1;
    std::stringstream ss2;
    std::stringstream ss3;
    
    ss0 << var;             // Expect this to fail, if it doesn't its an issue
    ss0 >> test;            // The var item bust be a string to set a variable
    if(!ss0.fail())
    {
        std::cout << "Error: Syntax error in for loop, first item can not be a raw value" << std::endl;
        exit(1);
    }
  
    ss1 << start;           // Test if start item is a variable or raw int
    ss1 >> test;
    if(ss1.fail())
    {
        startIsVar = true;
    }
    
    ss2 << end;             // Test if end item is a variable or raw int
    ss2 >> test;
    if(ss2.fail())
    {
        endIsVar = true;
    }
    
    if(step != "na")        // If a step item is given, test if its a raw int
    {
        ss3 << step;
        ss3 >> test;
        if(ss3.fail())
        {
            stepIsVar = true;
        }
    }

    // Translate the start / end / step items to their basm assignments
    std::string startTrans, endTrans, varTrans, stepTrans;
    
    // Get start translation
    if(startIsVar)
    {
        bool found = false;
        for(unsigned i = 0; i < variableList.size(); i++)
        {
            if(variableList[i].name == start)
            {
                found = true;
                varTrans = ("m" + ITOS(variableList[i].mi) + " ");
                
                // Set the variable = to start var info
                newVariable((var + " = " + start));
                
                // Now that varible is made, grab the translation
                bool found = false;
                for(unsigned i = 0; i < variableList.size(); i++)
                {
                    if(variableList[i].name == var)
                    {
                        found = true;
                        varTrans = ("m" + ITOS(variableList[i].mi) + " ");
                    }
                }
                if(!found)
                {
                    std::cout << "Error: Unable to find generated value for a forloop." << std::endl;
                    exit(1);
                }
            }
        }
        if(!found)
        {
            std::cout << "Error: Unknown start variable in for loop : " << statement << std::endl;
            exit(1);
        }
    }
    else
    {
        // Set the var = the data
        newVariable((var + " = " + ITOS(start)));
        
        // Now that varible is made, grab the translation
        bool found = false;
        for(unsigned i = 0; i < variableList.size(); i++)
        {
            if(variableList[i].name == var)
            {
                found = true;
                varTrans = ("m" + ITOS(variableList[i].mi) + " ");
            }
        }
        if(!found)
        {
            std::cout << "Error: Unable to find generated value for a forloop." << std::endl;
            exit(1);
        }
    }
    
    // Get start translation
    if(endIsVar)
    {
        bool found = false;
        for(unsigned i = 0; i < variableList.size(); i++)
        {
            if(variableList[i].name == end)
            {
                found = true;
                endTrans = ("m" + ITOS(variableList[i].mi) + " ");
            }
        }
        if(!found)
        {
            std::cout << "Error: Unknown end variable in for loop : " << statement << std::endl;
            exit(1);
        }
    }
    else
    {
        endTrans = ("$" + ITOS(end) + " ");
    }
    
    // Get step translation
    if(stepIsVar)
    {
        bool found = false;
        for(unsigned i = 0; i < variableList.size(); i++)
        {
            if(variableList[i].name == step)
            {
                found = true;
                stepTrans = ("m" + ITOS(variableList[i].mi) + " ");
            }
        }
        if(!found)
        {
            std::cout << "Error: Unknown step variable in for loop : " << statement << std::endl;
            exit(1);
        }
    }
    else
    {
        if(step != "na")
        {
            stepTrans = ("$" + ITOS(step) + " ");
        }
        else
        {
            stepTrans = ("$1 ");
        }
    }
    
    // Now that the translations are done, build the instructions
    
    // Create bottom label
    std::string forLoopBottomLabel = ("@_FOR_LOOP_BOT_" + ITOS(forLoopCounter));
    forLoopCounter++;
    
    // Create top Label
    std::string forLoopTopLabel = ("@_FOR_LOOP_TOP_" + ITOS(forLoopCounter));
    forLoopCounter++;
    
    // Add the branch command
    insertBasmComment("Prelim test");
    blangOutput.push_back("\tbge " + varTrans + endTrans + forLoopBottomLabel);
    
    // Add the 'top'
    blangOutput.push_back( "\t" + forLoopTopLabel);
    
    // Create the bottom commands
    std::string addInstruct = ("\tadd " + varTrans + varTrans + stepTrans + "\n");
    std::string branchInstruct = ("\tblt " + varTrans + endTrans + forLoopTopLabel + "\n");
    std::string appendLater = addInstruct + branchInstruct + "\t" + forLoopBottomLabel;
    
    // Add to stack for use when loop is complete
    forLoopEndCommands.push(appendLater);
}

/*
        CREATE END FOR LOOP
*/
void Bohll::endForLoop()
{
    if(BHLDEBUG)
        std::cout << "ENF FOR LOOP -> " << std::endl;
    
    if(forLoopEndCommands.empty())
    {
        std::cout << "Error: Stray for loop end found. " << std::endl;
        exit(1);
    }
    
    blangOutput.push_back(forLoopEndCommands.top());    // Append the commands for the bot of a forloop
    forLoopEndCommands.pop();                           // Remove current command
}

/*
        JUMP TO FUNCTION RETURN
*/
void Bohll::doDone()
{
    if(BHLDEBUG)
        std::cout << "DNE INSTRUCT -> " << std::endl;
    insertBasmComment("Done token -> jmp to bottom");
    blangOutput.push_back("\tjmp " + currentFunctionBottomLabel);
}

/*
        JUMP TO A LABEL
 */
void Bohll::doGo(std::string label)
{
    bool flag = false;
    std::string temp = "";
    for(unsigned i = 0; i < label.length(); i++)
    {
        if(label[i] == ' ')
            flag = true;
        
        if (flag)
            temp += label[i];
    }

    if(temp != "")
        label = temp;
    else
    {
        std::cout << "Error: No label given in go statement : " << label << std::endl;
        exit(1);
    }
    
    label = trim(label, ' ');
    label = trim(label, '\t');
    if('@' != label[0])
    {
        std::cout << "Error: Invalid label given in go statement : " << label << std::endl;
        exit(1);
    }
    blangOutput.push_back("\tjmp " + label);
}

/*
        CREATE DO WHILE LOOP
*/
void Bohll::doWhileLoop(std::string condition)
{
    if(BHLDEBUG)
        std::cout << "NEW DO WHILE -> " << condition << std::endl;
}

/*
        CREATE DO WHILE LOOP
*/
void Bohll::endDoWhileLoop()
{
    if(BHLDEBUG)
        std::cout << "END WHILE LOOP " << std::endl;
}

/*
        CREATE WHILE LOOP
*/
void Bohll::whileLoop(std::string condition)
{
    if(BHLDEBUG)
        std::cout << "WHILE LOOP -> " << condition << std::endl;
}

/*
        CREATE END WHILE LOOP
*/
void Bohll::endWhileLoop()
{
    if(BHLDEBUG)
        std::cout << "END WHILE LOOP" << std::endl;
}

/*
        CREATE IF STATEMENT
*/
void Bohll::ifStatement(std::string statement)
{
    if(BHLDEBUG)
        std::cout << "NEW IF " << statement << std::endl;
    
    unsigned flag = 0;
    std::string lhs = "", condition = "", rhs = "";                 // Obtain the left hand side, condition, and right hand side of if
    for(unsigned i = 3; i < statement.length(); i++)
    {
        if(statement[i] != ' ')
        {
            if(0 == flag)
            {
                lhs += statement[i];
            }
            else if (1 == flag)
            {
                condition += statement[i];
            }
            else if (2 == flag)
            {
                rhs += statement[i];
            }
        }
        else
        {
            flag++;
        }
    }
    
    // See if lhs and rhs is var or not
    
    int testVal;
    std::stringstream ss;
    std::stringstream ss1;
    
    // Since doubles can be converted to int, we perform that test
    
    bool lhsIsVar, rhsIsVar;
    ss << lhs;
    ss >> testVal;
    (ss.fail()) ? lhsIsVar = true : lhsIsVar = false;       // If it fails, its  avar
    
    ss1 << rhs;
    ss1 >> testVal;
    (ss1.fail()) ? rhsIsVar = true : rhsIsVar = false;      // If it fails, its a var
    
    std::string slhs = "", srhs = "";                       // Convert var to memdex, or val to $var
    
    // Get the lhs, and rhs of the branch command
    if(lhsIsVar)
    {
        bool found = false;
        for(unsigned i = 0; i < variableList.size(); i++)           // If its a var, get its memdex
        {
            if(variableList[i].name == lhs)
            {
                found = true;
                slhs = ("m" + ITOS(variableList[i].mi) + " ");
            }
        }
        if(!found)
        {
            if(lhs[0] != '"' || lhs[lhs.length()-1] != '"')
            {
                std::cout << "Error: If-Statement LHS unknown " << lhs << std::endl;
                exit(1);
            }
            slhs = (lhs + " ");
        }
    }
    else
    {
        slhs = ("$" + lhs + " ");                           // If its not, just add it in
    }
    
    if(rhsIsVar)
    {
        bool found = false;
        for(unsigned i = 0; i < variableList.size(); i++)           // If its a var, get its memdex
        {
            if(variableList[i].name == rhs)
            {
                found = true;
                srhs = ("m" + ITOS(variableList[i].mi) + " ");
            }
        }
        if(!found)
        {
            if(lhs[0] != '"' || lhs[lhs.length()-1] != '"')
            {
                std::cout << "Error: If-Statement RHS unknown " << rhs << std::endl;
                exit(1);
            }
            srhs = (rhs + " ");
        }
    }
    else
    {
        srhs = ("$" + rhs + " ");                           // If its not, just add it in
    }
    
    // Once the lhs and rhs are translated into slhs and sths, create label for branching
    
    std::string marker;                                     // Convert counter to string and append as marker to label
    
    std::stringstream ss2;
    ss2 << ifStatementCounter;
    ss2 >> marker;
    
    ifStatementCounter++;
    std::string ifStatementLabel = ("@_IF_STATEMENT_" + marker);
    
    ifStatementLabels.push("\t" + ifStatementLabel);        // Add the marker to stack for later placement
    
    std::string instruction = "";
    if(condition == "<")                                    // Less than
    {
        // Opposite of < is >=
        instruction = ("\tbge " + slhs + srhs + ifStatementLabel);
        
    }
    else if(condition == ">")                               // Greater than
    {
        // Opposite of > is <=
        instruction = ("\tble " + slhs + srhs + ifStatementLabel);
    }
    else if(condition == "<=")                              // Less than, or equal to
    {
        // Opposite of <= is >
        instruction = ("\tbgt " + slhs + srhs + ifStatementLabel);
    }
    else if(condition == ">=")                              // Greater than or equal to
    {
        // Opposite of >= is <
        instruction = ("\tblt " + slhs + srhs + ifStatementLabel);
    }
    else if(condition == "==")
    {
        // Opposite of == is ne
        instruction = ("\tbne " + slhs + srhs + ifStatementLabel);
    }
    else if(condition == "!=")
    {
        // Opposite of != is ==
        instruction = ("\tbeq " + slhs + srhs + ifStatementLabel);
    }
    else
    {
        std::cout << "Error: Unknown condition within if statement  : " << statement << std::endl;
        exit(1);
    }
    
    insertBasmComment("If statement branch test");
    
    blangOutput.push_back((instruction+"\n"));
}

/*
        END AN IF STATEMENT
 */
void Bohll::endIfStatement()
{
    if(ifStatementLabels.empty())
    {
        std::cout << "Error: No if statement to end. Stray fi instruction detected." << std::endl;
        exit(1);
    }
    std::string s = ifStatementLabels.top();
    blangOutput.push_back(s);
    ifStatementLabels.pop();
}

/*
        CREATE A PRINT INSTRUCTION
*/
void Bohll::print(std::string printExpression)
{
    if(BHLDEBUG)
        std::cout << "NEW PRINT " << std::endl;
    
    if(BHLCOMMENTCODE)
        insertBasmComment("Put item in temp memory slot");
    
    printState = true;
    newVariable(printStatementVariable + " = " + printExpression);
    printState = false;
    
    unsigned nmi = retrieveMemoryIndexOfVariable(printStatementVariable);
    
    blangOutput.push_back("\n\tp m"+ITOS(nmi));
    
    int j = -1;
    for(unsigned i = 0; i < variableList.size(); i++)
    {
        if(variableList[i].mi == nmi)
        {
            j = i;
        }
    }
    if(j == -1)
    {
        std::cout << "Error : Print variable not found!\n";
        exit(1);
    }
    
    if(BHLCOMMENTCODE)
        insertBasmComment("Removed print expression in temp memory slot");

    // Remove the print var
    //recycleMemory("free " + printStatementVariable);
    
    // Remove the print variable to save space
    variableList.erase(variableList.begin()+j);
    if(0 < csmi)
        csmi--;}

/*
        CREATE A READ INSTRUCTION
*/
void Bohll::read(std::string var)
{
    if(BHLDEBUG)
        std::cout << "READ -> " << var << std::endl;

    bool flag = false;
    std::string temp = "";
    for(unsigned i = 0; i < var.length(); i++)
    {
        if(var[i] == ' ')
            flag = true;
        
        if(flag)
            temp += var[i];
    }
    
    if(temp != "")
    {
        var= temp;
    }
    else
    {
        std::cout << "Error: No variable given for read in :" << var << std::endl;
        exit(1);
    }
    
    var = trim(var, ' ');
    
    // Create a variable for the read in
    newVariable((var + " = 0"));
    
    std::string varTrans;

    bool found = false;
    
    // Check existing variables
    for(unsigned i = 0; i < variableList.size(); i++)
    {
        if(variableList[i].name == var)
        {
            found = true;
            varTrans = ("m" + ITOS(variableList[i].mi) + " ");
        }
    }

    if(!found)
    {
        std::cout << "Error: Unable to set read variable : " << var << std::endl;
        exit(1);
    }
    
    insertBasmComment("Read in data from the user");
    blangOutput.push_back(("\tr " + varTrans));
    
}

/*
        CREATE A FILE OUTPUT INSTRUCTION
*/
void Bohll::file(std::string statement)
{
    if(BHLDEBUG)
        std::cout << "FILE -> " << statement << std::endl;
    
    unsigned flag = 0;
    std::string file = "", mode = "", source = "";                 // Obtain the left hand side, condition, and right hand side of if
    for(unsigned i = 5; i < statement.length(); i++)
    {
        if(statement[i] != ' ')
        {
            if(0 == flag)
            {
                file += statement[i];
            }
            else if (1 == flag)
            {
                mode += statement[i];
            }
            else if (2 == flag)
            {
                source += statement[i];
            }
        }
        else
        {
            flag++;
        }
    }
    
    // Full metal haircut
    file = trim(file, ' ');
    file = trim(file, '\t');
    file = trim(file, '\n');
    mode = trim(mode, ' ');
    mode = trim(mode, '\t');
    mode = trim(mode, '\n');
    source = trim(source, ' ');
    source = trim(source, '\t');
    source = trim(source, '\n');
    
    bool fileIsVar = false, sourceIsVar = false;
    std::string fileTrans, modeTrans, sourceTrans;
    
    // See if its a string or not
    if(file[0] != '"')
    {
        fileIsVar = true;
    }
    else
    {
        std::cout << "Error: Invalid file. Must be variable." << std::endl;
        exit(1);
    }
    
    if(source[0] != '"')
    {
        sourceIsVar = true;
    }
    else
    {
        std::cout << "Error: Invalid source. Must be variable." << std::endl;
        exit(1);
    }
    
    // Translate file to mem info
    if(fileIsVar)
    {
        bool found = false;
        for(unsigned i = 0; i < variableList.size(); i++)           // If its a var, get its memdex
        {
            if(variableList[i].name == file)
            {
                found = true;
                fileTrans = ("m" + ITOS(variableList[i].mi) + " ");
            }
        }
        if(!found)
        {
            std::cout << "Error: Unknown variable : " << file << " in statement : " << statement << std::endl;
            exit(1);
        }
    }
    else
    {
        fileTrans = file + " ";                                           // If its not, just add it in
    }
    
    // Translate output mode
    if(mode == "append")
        modeTrans = "a ";
    else if(mode == "write")
        modeTrans = "w ";
    else
    {
        std::cout << "Error: Invalid file mode : " << mode << ". In  statement : " << statement << std::endl;
        exit(1);
    }
    
    // Translate source
    if(sourceIsVar)
    {
        bool found = false;
        for(unsigned i = 0; i < variableList.size(); i++)           // If its a var, get its memdex
        {
            if(variableList[i].name == source)
            {
                found = true;
                sourceTrans = ("m" + ITOS(variableList[i].mi) + " ");
            }
        }
        if(!found)
        {
            std::cout << "Error: Unknown variable : " << source << " in if statement : " << statement << std::endl;
            exit(1);
        }
    }
    else
    {
        sourceTrans = source + " ";                                       // If its not, just add it in
    }
    insertBasmComment("File operation");
    blangOutput.push_back("\tf " + fileTrans + modeTrans + sourceTrans);
}

/*
        CREATE A LABEL  - COMPLETE
*/
void Bohll::label(std::string name)
{
    if(BHLDEBUG)
        std::cout << "NEW LABEL -> " << name << std::endl;
    
    blangOutput.push_back(name);
}

/*
        CREATE THE BASM FILE
*/
void Bohll::createBlangFle()
{
    if(BHLDEBUG)
        std::cout << "CREATE FILE" << std::endl;
    
    if(functionCounter != labelBottomCounter)
    {
        std::cout << "Error: There are un-ended functions in input file." << std::endl;
        exit(1);
    }
    
    
    std::ofstream ofs;
    ofs.open ("result.s", std::ofstream::out);
    if(ofs.fail())
    {
        std::cout << "Error writing result.s. Please check direectory permissions."
                  << std::endl;
        exit(1);
    }
    for(unsigned i = 0; i < blangOutput.size(); ++i)
    {
        ofs << blangOutput[i];
        ofs << "\n";
    }
    ofs.close();
}

/*
        ###############################################

                    PRIVATE SECTION
                        - HELPER METHODS
 
        ###############################################
 */

/*
        DIVIDE A STRING BY A CHAR
 */
std::vector<std::string> Bohll::splitString(char c, std::string str)
{
    std::vector<std::string> ret;                               // Create return item
    unsigned pivot = 0;
    bool inStr = false;
    for(unsigned i = 0; i < str.length(); i++)
    {
        if(str[i] == '"')
            (inStr) ? inStr = false : inStr = true;
        if(!inStr && str[i] == '=')                             // Allows = inside strings!
        {
            pivot = i;
            break;
        }
    }
    std::string a = "";
    for(unsigned i = 0; i < pivot; i++)
        a += str[i];
    std::string b = "";
    for(unsigned i = pivot+1; i < str.length(); i++)
        b += str[i];
    ret.push_back(a);
    ret.push_back(b);
    return ret;
}

/*
        REMOVE A CHAR FROM A STRING
 */
std::string Bohll::stripChar(char c, std::string str)
{                                                               // Take something out of a string
    std::string t = "";
    for(int i = 0; i < str.length(); i++)
    {
        if(str[i] != c)
            t += str[i];
    }
    return t;
}

/*
        TRIM A STRING ( RIGHT TRIM, LEFT TRIM, TRIM )
 */
std::string Bohll::rtrim(std::string s, char del)
{
    size_t endpos = s.find_last_not_of(del);
    return (endpos == std::string::npos) ? "" : s.substr(0, endpos+1);
}

// Trim left side of the string
std::string Bohll::ltrim(std::string s, char del)
{
    size_t startpos = s.find_first_not_of(del);
    return (startpos == std::string::npos) ? "" : s.substr(startpos);
}

// Calls left and right trim on the input
std::string Bohll::trim(std::string s, char del)
{
    return rtrim(ltrim(s, del), del);
}

/*
 
        DETERMINE THE DATATYPE OF AN ITEM
 
 */
int Bohll::analyzeDataType(std::string data)
{
    if(BHLDEBUG)
        std::cout << "IN ANALYZE DATATYPE : " << data << std::endl;
    
    // Some things we need to keep track of
    int dataType = -1;
    bool setAsFloat = false;
    
    // Explode the data so we can test each individual item
    ditems statementInformation = explodeStatement(data);
    for(int i = 0; i < statementInformation.items.size(); i++)
    {
        int anInt;
        std::stringstream ss(statementInformation.items[i]);
        ss >> anInt;                                                  // If item can be converted, its an int or float
        if(ss.fail())
        {
            for(int q = 0; q < variableList.size(); q++)              // Check the vars to see what its data type is
            {
                if(variableList[q].name == statementInformation.items[i])
                {
                    if(variableList[q].dtype == 's')                           // its a string
                    {
                        return STR_VAL;
                    }
                    else if(variableList[q].dtype == 'i' && !setAsFloat)       // its an int
                    {
                        dataType = INT_VAL;
                    }
                    else if(variableList[q].dtype == 'f')            // its a float
                    {
                        setAsFloat = true;
                    }
                }
            }
        }
        else
        {
            bool dec = false;                               // Check to see if the value contains a decimal
            for(int k = 0; k < statementInformation.items[i].length() && !dec; k++)
                if(statementInformation.items[i][k] == '.')
                    dec = true;
            if(dec)
                setAsFloat = true;                                  // If it has one, its a float
            else
                dataType = INT_VAL;                                 // If not, its an int
        }
    }
    
    if(setAsFloat)
        dataType = FLT_VAL;
    return dataType;
}

ditems Bohll::explodeStatement(std::string statement)
{
    ditems dataItems, ret;                                          // The return item
    std::vector<char> ops;                                          // Op list
    std::vector<std::string> items;                                 // Tokens
    std::string token = "";
    
    bool isString = false;                                          // Stest if its a string
    for(int i = 0; (i < statement.length() && !isString); i++)
        if(statement[i] == '"')
            isString = true;
    
    bool inString = false;
    
    for(int i = 0; i < statement.length(); i++)
    {
        if(statement[i] == '"')                                     // Toggle if we are scanning within "s
        {
            (!inString) ? inString = true : inString = false;
            token += statement[i];
            if(!inString && token != "")                            // Add the string item to the items list
            {
                items.push_back(token);
                token = "";
            }
        }
        else
        {
            if(!inString)                                           // If we aren't in "s, scan for ops
            {
                bool isOp = false;
                for(int j = 0; j  < 7 ; j++)
                {
                    if(statement[i] == mops[j])
                    {
                        ops.push_back(mops[j]);
                        isOp = true;                                // Signal its an op
                        j = 7;                                      // Quit searching for matching op
                        
                        items.push_back(
                                        stripChar(' ', token)
                                        );                          // Since we found an op, we can add the token
                        token = "";
                    }
                }
                
                if(!isOp)
                {
                    token += statement[i];
                }
            }
            else
            {
                token += statement[i];
            }
        }
    }
    token = trim(token, ' ');
    token = trim(token, '\t');
    items.push_back( stripChar(' ', token) );                       // Add the last item
    ret.items = items;                                              // Copy data, and return
    ret.ops = ops;
    return ret;
}

/*
        POSTFIX TRANSLATION
*/
std::string Bohll::postfix(std::string exp)
{
    std::stack<char> op;                                            // Stack for postfix conversion
    std::string res = "";
    std::map<char,std::pair<int,int> > ops;                         // Map for symbol handling priority
    ops['^'] = std::make_pair(4,1); // E
    ops['*'] = std::make_pair(3,0); // M
    ops['/'] = std::make_pair(3,0); // D
    ops['%'] = std::make_pair(3,0); // MOD
    ops['+'] = std::make_pair(2,0); // A
    ops['-'] = std::make_pair(2,0); // S
    
    for(unsigned i = 0; i < exp.size(); i++)                        // Go through string L to R
    {
        if(exp[i] == '(')                                           // Op
            op.push(exp[i]);                                        // Add to stack
        else if(exp[i] == ')')                                      // Cp
        {
            while(op.top() != '(')
            {
                res += op.top();                                    // Add to return string
                op.pop();
            }
            op.pop();
        }
        else if(ops.find(exp[i]) != ops.end())                      // Find op
        {
            while(!op.empty() &&
                ((ops[exp[i]].second == 0 &&
                  ops[op.top()].first == ops[exp[i]].first) ||
                  ops[exp[i]].first < ops[op.top()].first))
            {
                res += op.top();                                    // Add op to ret string
                op.pop();
            }
            op.push(exp[i]);
        }
        else
            res += exp[i];                                          // Add item to ret string
    }
    while(!op.empty())                                              // Add remaining ops
    {
        res += op.top();
        op.pop();
    }
    return res;                                                     // Retrun the postfix item
}

/*
            PUT SOME COMMENTS IN THE BASM FILE
 */
void Bohll::insertBasmComment(std::string comment )
{
    blangOutput.push_back("\t\t\t\t// " + comment);
}

/*
 ###############################################
 
            PRIVATE SECTION
                - CREATION METHODS
 
 ###############################################
 */

/*
        INPUT INTEGER INSTRUCTION
*/
void Bohll::constructString(std::string lhs, std::string rhs, bool hasOps)
{
    
    if(BHLDEBUG)
        std::cout << "\tIN CONSTRUCT STRING. :  " << lhs << " " << rhs << " "<< hasOps << std::endl;
    
    if(hasOps)
    {
        
        ditems items = explodeStatement(rhs);                               // Get string/ var items from line
        
        std::string nrhs = "", tmp = "";
        bool inStr = false;
        std::vector<std::string> strings;
        
        for(unsigned i = 0; i < items.items.size(); i++)                    // Trip off the fat, and add the itmes
        {
            if(stripChar(' ',items.items[i]).length() > 0)
                strings.push_back(items.items[i]);
        }
    
        nrhs = "";
        if(strings.size() < 2)
        {
            
            std::cout << "Error: Invalid number of strings for concatination!\n";
            exit(1);
        }
        
        nrhs = (strings[0] + " " + strings[1] + " + ");
        if(strings.size() > 2)
        {
            for(unsigned i = 2; i < strings.size();i++)
            {
                nrhs += (strings[i] + " + ");
            }
        }
        
        nrhs = trim(nrhs, ' ');
        setVariable('s', lhs, nrhs);
    }
    else                                                            // If there is no string operations, create the string variable
    {
        setVariable('s', lhs, rhs);
    }
}

void Bohll::constructNumber(std::string lhs, std::string rhs, bool hasOps)
{
    if(BHLDEBUG)
        std::cout << "\tIN CONSTRUCT NUMBER." << lhs << " " << rhs << " OPS :  " << hasOps << std::endl;
    
    if(hasOps)
    {
        bool flag;                                                  // Add some ws around operations
        std::string n = "";                                         // for post postfix readability
        for(int i = 0; i < rhs.length(); i++)
        {
            flag = false;
            for(int j = 0; j < 7 && !flag; j++)
            {
                if(mops[j] == rhs[i])                               // If find op
                {
                    flag = true;                                    // Say so
                    n += " ";                                       // Add some ws
                    n += rhs[i];
                    n += " ";
                }
            }
            if(!flag)
                n += rhs[i];                                        // If not an op, ad in the char
        }
        rhs = postfix(n);                                           // Convert to post fix
    }
    setVariable('n', lhs, rhs);
}

/*
 CONSTRUCT  A VARIABLE
 */
void Bohll::setVariable(char t, std::string name, std::string value)
{
    name = trim(name, ' ');
    name = trim(name, '\t');
    name = trim(name, '\n');
    value = trim(value, ' ');
    value = trim(value, '\t');
    value = trim(value, '\n');
    
    // if (t) == 'n' : number; else: string;
    if(BHLDEBUG)
        std::cout << "\n\t - SET VARIABLE : " << t << ". "
        << name << ", " << value << std::endl;
    char currDataType = 'u';
    int lineCount = 0;
    dataItems di = explodeStatement(value);
    
    int lhseavi = 0;                                                // See if updating lhs var or making a new
    bool lhseav = false;
    for(int z = 0; z < variableList.size() && !lhseav; z++)
        if(name == variableList[z].name)
        {
            lhseavi = z;
            lhseav = true;
        }
    
     /*
     
                CHECK TO SEE IF THE GIVEN VAR IS AN ARRAY
     
     */
    bool inBrack = false;
    bool firstBrack = true;
    std::string cV = "", cX = "", cY = "";
    unsigned openBrackets = 0, closedBrackets = 0;
    
    for(unsigned i = 0; i < name.length(); i++)
    {
        if(name[i] == '[')
        {
            inBrack = true;
            openBrackets++;
        }
        else if(name[i] == ']')
        {
            inBrack = false;
            firstBrack = false;
            closedBrackets++;
        }
        else if(inBrack)
        {
            if(firstBrack)
            {
                cX += name[i];          // Get X accessor
            }
            else
            {
                cY += name[i];          // Get Y accessor
            }
        }
        else
        {
            cV += name[i];              // Get variable name
        }
    }
    
    // If no brackets are found, we move in. If there were some found, lets do something with them
    if(openBrackets > 0 || closedBrackets > 0)
    {
        if (openBrackets != closedBrackets)
        {
            std::cout << "Error : Bracket usage found indicating array, but invalid pairs detected. Did you close all of your brackets?" << std::endl;
            exit(1);
        }
        else
        {
            // Send the information to have the array made
            setArrayVariable(t, cV, value, cX, cY);
        }
        // Once setArrayVariable called, we can return home.
        // The reason for this is, setArrayVariable calls setVariable with the information of sub-var in array
        // to get data set. Once this is done, there is nothing else to do.
        return;
    }
    
    /*
     
                IF VAR NOT ARRAY, BUSINESS AS USUAL
     
     */
    
    int tempCmi = 0;
    if(lhseav)
    {
        // If it is a variable, we will want to use the variable's memory index instead of a new one.
        // This gets restored at the bottom of the function. :D
        csmiControlFlag = true;
        tempCmi = csmi;
        csmi = variableList[lhseavi].mi;
    }
    
    // If the item is not a variable
    // We need to retrieve a memory index for it.
    int memoryAssignment = getNexMemoryIndex();
    
    // If there are no operations
    if(di.ops.size() == 0)
    {
        // Single item
        bvar s;                                                     // Assign variable information
        s.name = name;                                              // Name
        s.dtype = t;                                                // Variable type
        s.mi = memoryAssignment;                                    // Memory index (where it lives)
        
        int locationInVarList;
        bool existsAsVariable = false;                              // See if current var has already been used
        for(int i = 0; i < variableList.size(); i++)
        {
            if(variableList[i].name == stripChar(' ', value))
            {
                locationInVarList = i;                              // If it has, get its info
                existsAsVariable = true;
                i = variableList.size();
            }
        }
        
        if(existsAsVariable)                                        // If the variable exists, then update it
        {

            // Need to see if lhs is a var now
            bvar n;
            n = variableList[locationInVarList];
            n.name = name;
            n.mi = memoryAssignment;
            variableList.push_back(n);                              // Add to the existing variables list
            blangOutput.push_back("\tmov m" + ITOS(n.mi) +
                                  " m" + ITOS(variableList[locationInVarList].mi));
        }
        
        else
        {
            // If it doesnt exist, make it depending on the type
            if('n' == t)
            {
                blangOutput.push_back("\tmov m" + ITOS(memoryAssignment) + " $" + stripChar(' ', value) + "\n");
                
                std::stringstream ss;                               // Convert the item
                ss << value;
                ss >> s.ival;
                
                std::stringstream ss1;                              // Convert the other item
                ss1 << value;
                ss1 >> s.fval;
                
            }
            else if ('s' == t)
            {
                if(value == "__bohl_array_init_point__")
                {
                    // If the variable is an array initiation point, mark it as such.
                    s.ival = 1;
                    s.dtype = 'A';
                }
                blangOutput.push_back("\tmov m" + ITOS(memoryAssignment) + " " + value + "\n");
                std::string ephim = "";
                for(unsigned z = 1; z < value.length()-1; z++)
                {
                    ephim += value[z];
                }
                s.sval = ephim;
            }
            variableList.push_back(s);
        }
    }
    else
    {
        // Multiple items - Ignore strings here, they will be all set by now. . . trust me, I'm a Doctor.
        bvar s;                                                     // Assign variable information
        s.name = name;                                              // Name
        s.dtype = t;                                                // Variable type
        s.mi = memoryAssignment;                                 // Memory index (where it lives)
        s.ival = 0;                                                 // Init var data
        s.sval = "";
        s.fval = 0.0;
        
        blangOutput.push_back(("\t\t\t\t// Calculate: " + value + "\n") );
        std::string loadCommand = "mov ";
        bool done = false;                                          // Need to convert the postfix notation into its assembly form
        std::string tmp = "";
        std::queue<std::string> values;                             // Queue to hold each value that has to be analyzed
        
        // Go through the postfixed value, and figure out if each item is a var, or a raw value
        tmp = "";
        done = false;
        
        bool isString = false;
        for(int i = 0; i < value.length(); i++)
        {
            if(value[i] == '"')
                isString = true;
        
            bool isOp = false;
            for(int j = 0; j < 7 && !isOp; j++)
                if(mops[j] == value[i])
                    isOp = true;
            if(!isOp)                                               // Handle if it is an op or not
            {
                if(value[i] == ' ' && tmp != "")
                {
                    values.push(tmp);                               // Add
                    tmp = "";
                }
                else
                    tmp += value[i];                                // Add value
            }
            
            if (isOp)
            {
                values.push(tmp);                                   // Add
                tmp = value[i];
                values.push(tmp);
                tmp = "";
            }
        }
        
        if(isString)
        {
            // If the thing is a string, we need to build the queue differently
            std::queue<std::string> empty;
            std::swap(values, empty);
            
            tmp = "";
            bool inStr = false;
            for(unsigned i = 0; i < value.length(); i++)
            {
                if(value[i] == '"')
                    (inStr) ? inStr = false : inStr = true;
                
                if(!inStr && value[i] == ' ' && tmp != "")
                {
                    values.push(tmp);
                    tmp = "";
                }
                else if (!inStr && value[i] != ' ')
                {
                    tmp += value[i];
                }
                else if (inStr)
                {
                    tmp += value[i];
                }
            }
            values.push("+");
        }
        
        // Go through each value, and figure out if it is a variable, or not
        // If its a variable, load it from the stack into the operation,
        // If its a raw value, lda it into the operation
        std::string currentItem;        // Strings to hold the assembly line equivelant of operation
        std::string regLoadLine;        // Register load lines (lda, or whatever)
        std::stack<int> regLocs;        // Register locations
        int currReg = 10;               // Current register to use
        
        
        while(!values.empty())                                      // Step through queue
        {
            int ival;
            bool viop = false;
            
            if(isString)
                currentItem = values.front();
            else
                currentItem = stripChar(' ', values.front());           // Clean up the item
            currentItem = stripChar('\t', currentItem);
            currentItem = stripChar('\n', currentItem);
            if(currentItem == "")                                   // Ignore blank items
            {
                values.pop();
                continue;
            }
            
            for(int i =0; i < 7 && !viop; i++)                      // See if current item is an operation
                if(mops[i] == currentItem[0])
                    viop = true;
            
            if(viop)                                                // Figure out what operation it is
            {
                std::string regLoad = "\t";                         // Create the aseembly line associated
                
                switch(currentItem[0])
                {
                    case '^':
                    {
                         includeLibrary("[math]");
                         
                         int rhs = regLocs.top();                    // Get lhs (middle) and rhs register for command
                         regLocs.pop();
                         currReg--;
                         
                         int lhs = regLocs.top();
                         regLocs.pop();
                         currReg--;
                         
                         regLoad += ("mov r1 r" + ITOS(lhs));
                         regLoad += ("\n\tmov r2 r" + ITOS(rhs));
                         regLoad += ("\n\tcall __blib_power\t\t// Call the power funtion");
                         regLoad += ("\n\tmov r" + ITOS(currReg) + " r0\n");
                         
                         regLocs.push(currReg);                      // Add the newest result
                         currReg++;
                        
                         blangOutput.push_back(regLoad);
                         values.pop();                               // Pop current item from queue
                        
                        continue;
                        break;
                    }
                    case '%':
                    {
                        includeLibrary("[math]");
                        
                        int rhs = regLocs.top();                 // Get lhs (middle) and rhs register for command
                        regLocs.pop();
                        currReg--;
                        
                        int lhs = regLocs.top();
                        regLocs.pop();
                        currReg--;
                        
                        regLoad += ("mov r1 r" + ITOS(lhs));
                        regLoad += ("\n\tmov r2 r" + ITOS(rhs));
                        regLoad += ("\n\tcall __blib_modulus\t\t// Call the mod funtion");
                        regLoad += ("\n\tmov r" + ITOS(currReg) + " r0\n");
                        
                        regLocs.push(currReg);                      // Add the newest result
                        currReg++;
                        
                        blangOutput.push_back(regLoad);
                        values.pop();                               // Pop current item from queue
                        
                        continue;
                        break;
                    }
                    case '*':
                    {
                        regLoad += "mult ";
                    }
                        break;
                    case '+':
                    {
                        regLoad += "add ";;
                    }
                        break;
                    case '-':
                    {
                        regLoad += "sub ";
                    }
                        break;
                    case '/':
                    {
                        regLoad += "div ";
                    }
                        break;
                    default:
                        break;
                }
                int rhs = regLocs.top();                                // Get lhs (middle) and rhs register for command
                regLocs.pop();
                currReg--;
                
                int lhs = regLocs.top();
                regLocs.pop();
                currReg--;
                
                regLoad += ( "r" +  ITOS(currReg) + " r" + ITOS(lhs) + " r" + ITOS(rhs));
                regLocs.push(currReg);                                  // Add the newest result
                currReg++;
                
                blangOutput.push_back(regLoad);                         // Add the regLoad line to output
                values.pop();                                           // Pop current item from queue
                continue;
                
            }
            
            std::stringstream ss(currentItem);                          // Stream the current item
            ss >> ival;                                                 // Try making it an int
            if(ss.fail())                                               // If it can't me made into an int, its a variable
            {
                bool found = false;
                for(int i = 0; i < variableList.size() && !found; i++)
                {
                    if(variableList[i].name == currentItem)             // If its a variable, we need to load it from its place in memory
                    {
                        
                        found = true;
                   
                        regLoadLine = ("\tmov r" + ITOS(currReg) +
                                        " m" +
                                        ITOS(variableList[i].mi)
                                        );                           // Add to assembly line
                    }
                }
                
                if(!found && isString)
                {
                    
                    if(currentItem[0] != '"')
                       {
                       std::cout << "Error: Unknown variable/string [" << currentItem << "] in string operation." << std::endl;
                       exit(1);
                       }
                    
                    found = true;
                    regLoadLine = ("\tmov r" + ITOS(currReg) +
                                    " " +
                                    (currentItem)
                                    );                           // Add to assembly line
                }
                
                if(!found )                                              // If it is a var, but not found then we want to start the program on fire
                {
                    std::cout << ("Variable: " + currentItem + " does not exist.\n");
                    exit(1);
                }
                
                blangOutput.push_back(regLoadLine);                     // Add assembly line to file
                regLoadLine = "";                                       // Reset assembly line
                regLocs.push(currReg);                                  // Push the current load location
                currReg++;                                              // Add to counter
            }
            else                                                        // If it isn't a variable
            {                                                           // Add the assembly operation to load item into register
                                                                        //  Double check
                regLoadLine = ("\tmov r" + ITOS(currReg) +
                                   " $" + currentItem );
                blangOutput.push_back(regLoadLine);                  // Add line to output file
                
                regLoadLine = "";                                       // Reset the line
                regLocs.push(currReg);                                  // Add current load location
                currReg++;
            }
            values.pop();
        }   // End of While
        
        int nw = regLocs.top();                                         // New word location. At this point, with a corretly formatted expr, rLocations will have one entry remaining
        regLocs.pop();                                                  // Remove it
        currReg--;                                                      // Update the counter
        
        blangOutput.push_back("\n");
        
        
        blangOutput.push_back("\tmov m" + ITOS(memoryAssignment) + " r" + ITOS(nw) + "\t\t// Copy result into mem index \n");
        
        int lhseavi = 0;                                                // See if updating lhs var or making a new
        bool lhseav = false;
        for(int z = 0; z < variableList.size() && !lhseav; z++)         // If updating a var, delete the old one
            if(name == variableList[z].name)                            // We are about to write it back with its
            {                                                           // updated information, so we don't need it anymore
                lhseavi = z;
                lhseav = true;
            }
        if(lhseav)
            variableList.erase(variableList.begin() + lhseavi);
        
        variableList.push_back(s);                                      // Add var to list
    }
    
    if(lhseav)
    {
        csmi = tempCmi;
    }
    csmiControlFlag = false;
}

// Set some memory to be re-used
void Bohll::recycleMemory(std::string line)
{
    bool grab = false;
    std::string token = "";
    for(unsigned i =0; i < line.length(); i++)
    {
        if(' ' == line[i])
        {
            if(grab == true)
            {
                std::cout << "Error: Multiple items found in free instruction" << std::endl;
                exit(1);
            }
            else
            {
                grab = true;
            }
        }
        if(grab)
        {
            token += line[i];
        }
    }
    token = stripChar(' ', token);
    token = trim(token, ' ');
    token = trim(token, '\t');
    token = trim(token, '\n');
    
    //std::cout << "Looking to free var : " << token << std::endl;
    unsigned index;
    bool found = false;
    for(unsigned i = 0; i < variableList.size(); i++)
    {
            if(token == variableList[i].name)
            {
                index = i;
                found = true;
                
                // Remove the print variable to save space
                //   variableList.erase(variableList.begin()+j);
                recycledMemory.push(variableList[i].mi);                // Add the memory index to be recycled
  
            }
    }
    if(!found)
    {
        std::cout << "Error: Attempted to free : " << token << ", but could not find its memory index. [HALT] " << std::endl;
        exit(1);
    }

    
    insertBasmComment("Freed memory item [m" + ITOS(variableList[index].mi) + "]\n");
    variableList.erase(variableList.begin() + index);                   // take out of the memory list
}

// Gets a new memory index
int Bohll::getNexMemoryIndex()
{
    /*
     
            If there are memory slots that have been recycled, their addresses will be
            distributed out to new assignments in the order that they were recycles.
            If there are no recycled addresses, the current memory index is ues
     
     */
    
    if(csmiControlFlag)
        return csmi;
    
    if(!recycledMemory.empty())
    {
        varAssignFlag = true;
        int ret = recycledMemory.front();
        csmi = ret;
        recycledMemory.pop();

        if(BHLDEBUG)
            insertBasmComment("Assigning new variable with recycled index : " + ITOS(ret));
        
        return ret;
    }
    else
    {
        varAssignFlag = false;
        return csmi++;
    }
}

// Bring in a LL Library - Copies to basm file
void Bohll::includeLibrary(std::string lib)
{
    std::fstream file;
    file.open(LIBRARY_LOCATION + lib);
    
    if (std::find(includedLibs.begin(), includedLibs.end(), lib) == includedLibs.end())
    {
        if(file.fail())
        {
            std::cout << "Error : Unable to find include : Library/" + lib  << std::endl;
            exit(1);
        }
        else
        {
            std::string line = "";
            std::vector<std::string> inLib;
            while(std::getline(file, line))
            {
                inLib.push_back(line);
            }
            std::vector<std::string>::iterator iter = blangOutput.begin();
            blangOutput.insert(iter, inLib.begin(), inLib.end());
        }
        file.close();
        includedLibs.push_back(lib);
    }
}

void Bohll::setArrayVariable(char type, std::string name, std::string value, std::string x1, std::string y1)
{
    // Find memory index in variable, calculate the actual item. Find its generated name, and call setVariable with given data
    // do set the arr sub index data
    /*
            x1 and y1 mark the index of the item we want to re-assign. The size x, and y of the array is stored in variableList[i].x , .y
     
     */
    
    unsigned ami = 0;                                   // Array memory index
    bool arrFound = false;
    bool dArray = false;
    for(unsigned i = 0; i < variableList.size(); i++)
    {
        if(variableList[i].name == name)
        {
            arrFound = true;
            ami = variableList[i].mi;
            if(variableList[i].arrType == 'D')
                dArray = true;
            
            std::cout << "2132 : " << variableList[i].name << " " << variableList[i].mi  << " | IS DOUBLE ?  " << dArray << std::endl;
        }
    }
    if(!arrFound)
    {
        std::cout << "Error : Unknown array : " << name << "| IN : " << "setArrayVariable() >> " << name << std::endl;
        exit(1);
    }
    arrFound = false;
    
    // Now calculate the mem index
    // Re-use ami as the index for the sub array. Because why make more variables?
    
    // Attempt to translate x1 and y1 to unsigned. If fail, assume var. If var. get val. If err, die
    
    unsigned X1 = 0, Y1 = 0;
    std::stringstream ss;
    ss << x1;
    ss >> X1;
    
    // If x trans fail, see if is var
    if(ss.fail())
    {
        bool a = false;
        for(unsigned i = 0; i < variableList.size(); i++)
        {
            if(variableList[i].name == x1)
            {
                a = true;
                X1 = variableList[i].ival;
            }
        }
        if(!a)
        {
            std::cout << "Error: Unknown variable in array X-Accessor : " << x1 << std::endl;
            exit(1);
        }
    }
    
    if( X1 > variableList[ami].x+1)
    {
        std::cout << "Error : X-Accessor out of bounds : " << name << std::endl;
        exit(1);
    }
    
    if(dArray)
    {
        std::stringstream ss1;
        ss1 << y1;
        ss1 >> Y1;
    
        // If y trans fail, see if is var
        if(ss1.fail())
        {
            bool a = false;
            for(unsigned i = 0; i < variableList.size(); i++)
            {
                if(variableList[i].name == y1)
                {
                    a = true;
                    Y1 = variableList[i].ival;
                }
            }
            if(!a)
            {
                std::cout << "Error: Unknown variable in array Y-Accessor : " << y1 << std::endl;
                exit(1);
            }
        }
        if( Y1 > variableList[ami].y+1)
        {
            std::cout << "Error : Y-Accessor out of bounds : " << name << std::endl;
            exit(1);
        }
    }
    
    // Find the element of the array
    if(dArray)
        ami = ami + ((variableList[ami].x+1) * Y1 + X1 + 1);
    else
        ami = (ami + X1 + 1);
    
    for(unsigned i = 0; i < variableList.size(); i++)
    {
        if(variableList[i].mi == ami)
        {
            
            
            
            
            std::cout << "2222: LOOKING FOR : " << ami << " CHECKING  : " << variableList[i].mi << " OWNDER :" << variableList[i].name << std::endl;
            
            
            
            arrFound = true;
        }
    }
    if(!arrFound)
    {
        std::cout << "Error : Unknown array index. Possible mis-translated ami : " << ami << std::endl;
        exit(1);
    }
    setVariable(type, variableList[ami].name, value);
}

/*
        Send a name here to get its index. If its arr[X][Y], or arr[X]
 */
unsigned Bohll::retrieveMemoryIndexOfVariable(std::string name)
{
    bool inBrack = false;
    bool firstBrack = true;
    std::string cV = "", cX = "", cY = "";
    unsigned openBrackets = 0, closedBrackets = 0;
    for(unsigned i = 0; i < name.length(); i++)
    {
        if(name[i] == '[')
        {
            inBrack = true;
            openBrackets++;
        }
        else if(name[i] == ']')
        {
            inBrack = false;
            firstBrack = false;
            closedBrackets++;
        }
        else if(inBrack)
        {
            if(firstBrack)
            {
                cX += name[i];          // Get X accessor
            }
            else
            {
                cY += name[i];          // Get Y accessor
            }
        }
        else
        {
            cV += name[i];              // Get variable name
        }
    }
    
    // If no brackets are found, we move in. If there were some found, lets do something with them
    if(openBrackets > 0 || closedBrackets > 0)
    {
        if (openBrackets != closedBrackets)
        {
            std::cout << "Error : Bracket usage found indicating array, but invalid pairs detected. Did you close all of your brackets?" << std::endl;
            exit(1);
        }
        else
        {
            // Send the information to have the array made
            //setArrayVariable(t, cV, value, cX, cY);
            bool dArray;
            bool arrFound = false;
            unsigned ami = 0;
            unsigned X1 = 0, Y1 = 0;
            for(unsigned i = 0; i < variableList.size(); i++)
            {
                
                if(variableList[i].name == cV)
                {
                    std::cout << "NAME : " << cV << " " << "TYPE : " << variableList[i].arrType << " " << "VNAME : " << variableList[i].name << std::endl;
                    
                    switch(variableList[i].arrType)
                    {
                            
                        case 'S':
                        {
                            arrFound = true;
                            dArray = false;
                            ami = variableList[i].mi;
                            std::stringstream ss;
                            ss << cX;
                            ss >> X1;
                            break;
                        }
                        case 'D':
                        {
                            arrFound = true;
                            dArray = true;
                            ami = variableList[i].mi;
                            std::stringstream ss;
                            ss << cX;
                            ss >> X1;
                            std::stringstream ss1;
                            ss1 << cY;
                            ss1 >> Y1;
                            break;
                        }
                        default:
                            std::cout << "Error accessing array variable in retrieveMemoryIndexOfVariable(" << cV << ")\n";
                            exit(1);
                            break;
                    }
                }
                
                
                std::cout << "LOOKING FOR : " << cV << " | CHECKING AGAINST : " << variableList[i].name << std::endl;
            }
            
            if(!arrFound)
            {
                std::cout << "Error : Unable to find given array : " << name << std::endl;
                exit(1);
            }
            
            
            // Find the element of the array
            if(dArray)
                ami = ami + ((variableList[ami].x+1) * Y1 + X1 + 1);
            else
                ami = (ami + X1 + 1);
            
            arrFound = false;
            for(unsigned i = 0; i < variableList.size(); i++)
            {
                if(variableList[i].mi == ami)
                {
                    arrFound = true;
                }
            }
            if(!arrFound)
            {
                std::cout << "Error : Unknown array index. Possible mis-translated ami in retrieveMemoryIndexOfVariable: " << ami << std::endl;
                exit(1);
            }
            
            // Send back the memory index of the sub-variable accessed
            return ami;
        }
    }
    else
    {
        // Its a regular variable, find it, and send its memdex
        for(unsigned i = 0; i < variableList.size(); i++)
        {
            if(variableList[i].name == name)
            {
                return variableList[i].mi;
            }
        }
        std::cout << "Error : Unknown variable :  " << name << std::endl;
        exit(1);
    }
}


