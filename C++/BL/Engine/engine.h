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

#ifndef BLANG_ENGINE
#define BLANG_ENGINE

namespace blang
{
                            // How many registers to use
#define ENG_REG_COUNT 60
                            // How many commands exist
#define PRIMARY_COMMANDS 16
                            // Debug / Run-time flags
#define BDEBUG 0
#define DISPLAY_RUN_TIME 0
#define MEM_DUMP_BEFORE_PROCESS 0
                            // Instruction codes - NEVER.EVER.CHANGE.THESE
#define INSTRUCT_MULT   0
#define INSTRUCT_DIV    1
#define INSTRUCT_ADD    2
#define INSTRUCT_SUB    3
#define INSTRUCT_JMP    4
#define INSTRUCT_CALL   5
#define INSTRUCT_MOV    6

#define INSTRUCT_BLE    7
#define INSTRUCT_BNE    8
#define INSTRUCT_BGT    9
#define INSTRUCT_BLT    10

#define INSTRUCT_P      11
#define INSTRUCT_READ   12
#define INSTRUCT_FILE   13

#define INSTRUCT_BEQ    14
#define INSTRUCT_BGE    15

#define INSTRUCT_LABEL  99


/*

        OPERATION FRAMEWORK

*/
// Structure for storing 'memory' shared between functions
struct memslot
{
    long long i;                    // Num storage
    bool b;                         // Bool storage
    double f;                       // Float storage
    std::string s;                  // String storage
    char type;                      // Type of mem for access
};

typedef struct memslot mslt;

// Instruction to be executed
struct instruction
{
    unsigned operation;         // Operation of instruction
    mslt addrSlot1;             // Storage for address (r*) if 'r' ->localmem's int, 'm'-> localmem's int
    mslt addrSlot2;             // Storage for address (r*, m*, s*, $*) s* // Local mems values will flag registers, and save used strings
                                //  'r' ->localmem's int , 'm'-> localmem's int, 's'-> localmem's string
    mslt addrSlot3;
    unsigned selfAddress;       // Instruction's location within frame's isntructions set
    unsigned callAddress;       // Address it might call
};

typedef struct instruction ins;

//typedef std::vector<ins> inslist;

// Function Framework
struct functionFrame
{
    unsigned fMemLoc;               // Location in frame memory
    bool isEntry;
    unsigned caller;
    unsigned callerInsc;            // The instruction counter of the caller function
    std::vector<ins> insSet;    // Instruction set of function - Engine's insc will traverse this
};

typedef struct functionFrame ffrm;

/*

        ENGINE STRUCTURE

*/
struct engine
{
    mslt       rMem[ENG_REG_COUNT]; // Run time memory - The 'function-shared memory space'registers'- primary use is passing params
    std::vector<mslt>     sMem;     // Shared memory. The memory shared by all function
    std::vector<ffrm>     fMem;     // Function mem - Memory to contain program's functions set
    std::stack<ffrm>      fStack;   // The current operating stack. When frame completed, erase the shared memory it allocated
    unsigned              insc;     // Instruction counter. This will mark the location of the current instruction being
                                    // executed. Instructions such as jmp, or branch will edit this value to move execution
    unsigned              cfc;      // Current frame counter - Index of frame in fMem that is being executed
    mslt                  funcRet;  // Temp storage for last exited function. Once function completes, copy its locMem here,
                                    // so the caller can retrieve data.
};

typedef struct engine eng;

/*

    ENGINE OPERATIONS

*/

/*
            HELPER FUNCTIONS FOR ENGINE
*/
// Dump memory contents
void memory_dump(eng &beng)
{
    std::cout << "\n\n\t--- MEMORY DUMP ---\n" << std::endl;
    for(unsigned j = 0; j < beng.fMem.size(); ++j)                                      // Go through all frames
    {
        std::cout << "\n\t--- FRAME: " << j << " ---" << std::endl;
        for(unsigned k = 0; k < beng.fMem[j].insSet.size(); ++k)                        // Go through all instructions
        {
            std::cout << "  INSTRUCTION SET: " << k << std::endl<< std::endl;
            std::cout << "\tOP: " << beng.fMem[j].insSet[k].operation << std::endl;
            std::cout << "\tADDR1 TYPE: " << beng.fMem[j].insSet[k].addrSlot1.type << std::endl;
            switch(beng.fMem[j].insSet[k].addrSlot1.type)
            {
            case 'r':
                std::cout << "\tADDR1 VAL: " << beng.fMem[j].insSet[k].addrSlot1.i << std::endl;
                break;
            case 'm':
                std::cout << "\tADDR1 VAL: " << beng.fMem[j].insSet[k].addrSlot1.i << std::endl;
                break;
            case 'i':
                std::cout << "\tADDR1 VAL: " << beng.fMem[j].insSet[k].addrSlot1.i << std::endl;
                break;
            case 'f':
                std::cout << "\tADDR1 VAL: " << beng.fMem[j].insSet[k].addrSlot1.f << std::endl;
                break;
            case 's':
                std::cout << "\tADDR1 VAL: " << beng.fMem[j].insSet[k].addrSlot1.s << std::endl;
                break;
            case 'c':
                std::cout << "\tCALL THE ADDR: " << beng.fMem[j].insSet[k].addrSlot1.i << std::endl;
                break;
            case 'l':
                std::cout << "\tLABEL ID: " << beng.fMem[j].insSet[k].addrSlot1.i << std::endl;
                break;
            case 'j':
                std::cout << "\tJMP: " << beng.fMem[j].insSet[k].addrSlot1.i << std::endl;
                break;
            case 'b':
                // Branch has no special info here
                break;
            default:
                std::cout << "ERROR: blang::process->mem dump->addr1 [" << beng.fMem[j].insSet[k].addrSlot1.type << "]" <<std::endl;
                exit(1);
            }

            std::cout << std::endl;
            std::cout << "\tOP: " << beng.fMem[j].insSet[k].operation << std::endl;
            std::cout << "\tADDR2 TYPE: " << beng.fMem[j].insSet[k].addrSlot2.type << std::endl;
            switch(beng.fMem[j].insSet[k].addrSlot2.type)
            {
            case 'r':
                std::cout << "\tADDR2 VAL: " << beng.fMem[j].insSet[k].addrSlot2.i << std::endl;
                break;
            case 'm':
                std::cout << "\tADDR2 VAL: " << beng.fMem[j].insSet[k].addrSlot2.i << std::endl;
                break;
            case 'i':
                std::cout << "\tADDR2 VAL: " << beng.fMem[j].insSet[k].addrSlot2.i << std::endl;
                break;
            case 'f':
                std::cout << "\tADDR2 VAL: " << beng.fMem[j].insSet[k].addrSlot2.f << std::endl;
                break;
            case 's':
                std::cout << "\tADDR2 VAL: " << beng.fMem[j].insSet[k].addrSlot2.s << std::endl;
                break;
            case 'c':
                std::cout << "\tRET TO FUNC: " << beng.fMem[j].insSet[k].addrSlot2.i << std::endl;
                break;
            case 'l':
                std::cout << "\tLABEL LOCATION: " << beng.fMem[j].insSet[k].addrSlot2.i << std::endl;
                break;
            case 'j':
                std::cout << "\tJMP: Does not use slot 2" << std::endl;
                break;
            case 'b':
                // No special data here
                break;
            default:
                std::cout << "ERROR: blang::process->mem dump->addr2 [" << beng.fMem[j].insSet[k].addrSlot2.type << "]" <<std::endl;
                exit(1);
            }

            std::cout << std::endl;
            std::cout << "\tOP: " << beng.fMem[j].insSet[k].operation << std::endl;
            std::cout << "\tADDR3 TYPE: " << beng.fMem[j].insSet[k].addrSlot3.type << std::endl;
            switch(beng.fMem[j].insSet[k].addrSlot3.type)
            {
            case 'r':
                std::cout << "\tADDR3 VAL: " << beng.fMem[j].insSet[k].addrSlot3.i << std::endl;
                break;
            case 'm':
                std::cout << "\tADDR3 VAL: " << beng.fMem[j].insSet[k].addrSlot3.i << std::endl;
                break;
            case 'i':
                std::cout << "\tADDR3 VAL: " << beng.fMem[j].insSet[k].addrSlot3.i << std::endl;
                break;
            case 'f':
                std::cout << "\tADDR3 VAL: " << beng.fMem[j].insSet[k].addrSlot3.f << std::endl;
                break;
            case 's':
                std::cout << "\tADDR3 VAL: " << beng.fMem[j].insSet[k].addrSlot3.s << std::endl;
                break;
            case 'c':
                std::cout << "\tAT INSTRUCT: " << beng.fMem[j].insSet[k].addrSlot3.i << std::endl;
                break;
            case 'l':
                std::cout << "\tLABEL: s3 unused: " << beng.fMem[j].insSet[k].addrSlot3.i << std::endl;
                break;
            case 'j':
                std::cout << "\tJMP: Does not use slot 3" << std::endl;
                break;
            case 'b':
                std::cout << "\tBRANCH TO: "   << beng.fMem[j].insSet[k].addrSlot3.i << std::endl;
                std::cout << "\tBRANCH TYPE: " << beng.fMem[j].insSet[k].addrSlot3.f << std::endl;
                break;
            default:
                std::cout << "ERROR: blang::process->mem dump->addr3 [" << beng.fMem[j].insSet[k].addrSlot3.type << "]" <<std::endl;
                exit(1);
            }
            std::cout << " - - - - - - - - - - " << std::endl;
        } // End for
    } // End for
}

// Create new instruction for current frame being created
void create_instruction(eng &engine, unsigned cmdId, mslt s1, mslt s2, mslt s3)
{
    // Add a new instruction set for next instruction
    ins newInsList;                                      // New instruction set list
    engine.fMem[engine.cfc].insSet.push_back(newInsList);    // Give the frame a new instuction set
    engine.insc = engine.fMem[engine.cfc].insSet.size()-1;   // Use insc to keep track of instruction set input for frame

    // Create the new instruction
    ins newInstruction;
    newInstruction.operation = cmdId;
    //newInstruction.callAddress = callBack;

    // Configure slot 1
    newInstruction.addrSlot1.type = s1.type;    // Configure slot 1 - type
    if(s1.type == 'f')                          // Configure slot 1 = value
        newInstruction.addrSlot1.f = s1.f;
    if (s1.type == 'i' || s1.type == 'm' || s1.type == 'r' || s1.type == 'c'
              || s1.type == 'l'  || s1.type == 'j' || s1.type == 'b')
        newInstruction.addrSlot1.i = s1.i;
    if (s1.type == 's' || s1.type == 'j')
        newInstruction.addrSlot1.s = s1.s;

    // Configure slot 2
    newInstruction.addrSlot2.type = s2.type;    // Configure slot 2 - type
    if(s2.type == 'f')                          // Configure slot 2 = value
        newInstruction.addrSlot2.f = s2.f;
    if (s2.type == 'i' || s2.type == 'm' || s2.type == 'r' || s2.type == 'c'
             || s2.type == 'j' || s2.type == 'b')
        newInstruction.addrSlot2.i = s2.i;
    if (s2.type == 's')
        newInstruction.addrSlot2.s = s2.s;
    if (s2.type == 'l')
        newInstruction.addrSlot2.i = engine.insc;
    if(s2.type == 'F')
        newInstruction.addrSlot2.b = s2.b;

    // Configure slot 3
    newInstruction.addrSlot3.type = s3.type;    // Configure slot 2 - type
    if(s3.type == 'f'|| s3.type == 'b')         // Configure slot 2 = value
        newInstruction.addrSlot3.f = s3.f;
    if (s3.type == 'i' || s3.type == 'm' || s3.type == 'r' || s3.type == 'c'
             || s3.type == 'l'  || s3.type == 'j' || s3.type == 'b')
        newInstruction.addrSlot3.i = s3.i;
    if (s3.type == 's' || s3.type == 'b')
        newInstruction.addrSlot3.s = s3.s;
    if(s3.type == 'F')
        newInstruction.addrSlot3.b = s3.b;

    // Insert Instruction set into memory - Would've done an op overload, but this is the only time it has to happen
    engine.fMem[engine.cfc].insSet[engine.insc].operation = newInstruction.operation;
    engine.fMem[engine.cfc].insSet[engine.insc].addrSlot1 = newInstruction.addrSlot1;
    engine.fMem[engine.cfc].insSet[engine.insc].addrSlot2 = newInstruction.addrSlot2;
    engine.fMem[engine.cfc].insSet[engine.insc].addrSlot3 = newInstruction.addrSlot3;
    engine.fMem[engine.cfc].insSet[engine.insc].selfAddress = newInstruction.selfAddress;
    engine.fMem[engine.cfc].insSet[engine.insc].callAddress = newInstruction.callAddress;
}

/*

            ENGINE FUNCTIONALITY

*/

/*
            ADD MEMORY SLOT TO MEMORY - Complete
*/
void engine_mem_add(eng &beng, mslt &memSlot, unsigned index)
{
    if(BDEBUG)
        std::cout << "ENGINE_MEM_ADD \t\t: m" << index << std::endl;

    // If index is bigger than slot, expand memory
    if(index > beng.sMem.size()-1)
    {
        for(unsigned i = beng.sMem.size()-1; i != index; ++i )
        {
            // Initialize a new memory slot... the hard way.
            mslt n; n.type = 'n'; n.f = 0.0; n.i = 0; n.s = ""; n.b = false;
            // Add the new slot to the memory unit
            beng.sMem.push_back(n);
        }
    }
    // Copy over data.
    beng.sMem[index].type = memSlot.type;
    beng.sMem[index].s = memSlot.s;
    beng.sMem[index].f = memSlot.f;
    beng.sMem[index].b = memSlot.b;
    beng.sMem[index].i = memSlot.i;
}

/*
            RETRIEVE DATA FROM MEMORY SLOT - Complete
*/
mslt engine_mem_get(eng &beng, unsigned index)
{
    // Here we assume the user knows what they are looking for,
    // as a reals sytem would not hold their hand
    return beng.sMem[index];
}

/*
            ADD MEMORY SLOT TO REGISTER - Complete
*/
void engine_reg_add(eng &beng, mslt &memSlot, unsigned index)
{
    if(BDEBUG)
        std::cout << "ENGINE_REG_ADD \t\t: r" << index << std::endl;

    // Make sure that the index sent in isn't out of range.
    // Thankfully unsigned's don't go negative. :D
    if(ENG_REG_COUNT < index)
    {
        std::cout << "Engine Error [engine_reg_add()] : Invalid Register Index"
                  << std::endl;
        exit(1);
    }
    // Copy over data.
    beng.rMem[index].type = memSlot.type;
    beng.rMem[index].s = memSlot.s;
    beng.rMem[index].f = memSlot.f;
    beng.rMem[index].b = memSlot.b;
    beng.rMem[index].i = memSlot.i;
}

/*
            RETRIEVE DATA FROM REGISTER - Complete
*/
mslt engine_reg_get(eng &beng, unsigned index)
{
    // Make sure that the index sent in isn't out of range.
    // Thankfully unsigned's don't go negative. :D
    if(ENG_REG_COUNT < index)
    {
        std::cout << "Engine Error [engine_reg_get()] : Invalid Register Index"
                  << std::endl;
        exit(1);
    }
    return beng.rMem[index];
}

/*
            LOAD FRAME ONTO STACK - Complete
*/
void engine_stack_load(eng &beng, unsigned index, bool flag)
{
    /*
            Find the item in fmem.
            Set that item's callerInsc to the engine's insc
                this way we can recover from where we left off
            Reset the engine's insc, as we want the first instruct
            of the function
            set the engine's cfc to the index of the callee
    */

    // Make sure frame is within range
    if(index > beng.fMem.size()-1)
    {
        std::cout << "Engine Error: [engine_stack_load()] : Invalid Frame Index"
                  << std::endl;
        exit(1);
    }

    // If the frame being added isn't the initial frame (main) :
    // Set the new frame's callerInsc to the current instruction counter
    // This is so we can recall what instruction we left off at.
    if(flag)
    {
        beng.fMem[index].callerInsc = beng.insc;    // Set the caller insc as the current insc
        beng.fMem[index].isEntry = false;
    }
    else
    {
        beng.fMem[index].callerInsc = beng.insc;     // Set the caller as its self
        beng.fMem[index].isEntry = true;
    }

    // Set the 'caller' of the function
    beng.fMem[index].caller = beng.cfc;

    // Since we are loading a new frame set the instruction counter to 0
    beng.insc = 0;

    // Set the current function counter to the calllee ( the one to be loaded )
    beng.cfc = index;

    // Add the frame at the index onto the stack
    beng.fStack.push(beng.fMem[beng.cfc]);
}

/*
            POP CURRENT FRAME FROM STACK - Complete
*/
void engine_stack_pop(eng &beng)
{
    /*
            Grab the current stack item's callernsc
                this is so we can restore engine's insc
                and continue on the next frame where we left off

            Pop the item off the stack.
            Set current frame counter to the current frame's
            fMemLoc
            -- maybe some other things
    */
    // If the stack is empty, we have a problem
    if(beng.fMem.empty())
    {
        std::cout << "Engine Error : [engine_stack_pop] : Attempting to pop empty stack"
                 << std::endl;
        exit(1);
    }

    // If the item is Entry point
    if(beng.fMem[beng.cfc].isEntry)
    {
        // Remove the item. We're all done here.
        beng.fStack.pop();
    }
    else    // If the item isn't an entry point, we will have to do a few things
    {
        // Restore the instruction counter from the current function so we can pick up where we left off
        beng.insc = ++beng.fMem[beng.cfc].callerInsc;
        beng.cfc = beng.fMem[beng.cfc].caller;
        beng.fStack.pop();
    }
}

/*
            PERFORM THE ARITHMATIC - Complete
*/
void engine_op_alu(eng &beng)
{
    if(BDEBUG)
        std::cout << "\tengine_op_alu(beng);" << std::endl;

    // Create a result mem slot
    mslt result;

    // Left and right hand side of equations
    long long ilhs, irhs;
    double flhs, frhs;
    std::string slhs, srhs;
    bool stringFlagS2 = false;


    // Figure out if floats are around. . .
    bool floatFlagS2 = false; // Assume not a float

    /*
            GET LHS VAL
    */
    // Check first slot for existence of floats in s2
    if('f' == beng.fMem[beng.cfc].insSet[beng.insc].addrSlot2.type)
    {
        // Set flag and get lhs for mathing later
        flhs = beng.fMem[beng.cfc].insSet[beng.insc].addrSlot2.f;
        floatFlagS2 = true;
    }   // This checking the contents of a register
    else if ('r' == beng.fMem[beng.cfc].insSet[beng.insc].addrSlot2.type)
    {
        if('f' == beng.rMem[beng.fMem[beng.cfc].insSet[beng.insc].addrSlot2.i].type)
        {
            // Grab the lhs from the reg
            flhs = beng.rMem[beng.fMem[beng.cfc].insSet[beng.insc].addrSlot2.i].f;
            floatFlagS2 = true;
        }
        else if ('i' == beng.rMem[beng.fMem[beng.cfc].insSet[beng.insc].addrSlot2.i].type)
        {
            ilhs = beng.rMem[beng.fMem[beng.cfc].insSet[beng.insc].addrSlot2.i].i;
        }
        else if('s' == beng.rMem[beng.fMem[beng.cfc].insSet[beng.insc].addrSlot2.i].type)
        {
            stringFlagS2 = true;
            slhs = beng.rMem[beng.fMem[beng.cfc].insSet[beng.insc].addrSlot2.i].s;
        }
    }   // This is checking the contents of a mem slot
    else if ('m' == beng.fMem[beng.cfc].insSet[beng.insc].addrSlot2.type)
    {
        if('f' == beng.sMem[beng.fMem[beng.cfc].insSet[beng.insc].addrSlot2.i].type)
        {
            // Grab the lhs from the mem slot
            flhs = beng.sMem[beng.fMem[beng.cfc].insSet[beng.insc].addrSlot2.i].f;
            floatFlagS2 = true;
        }
        else if('i' == beng.sMem[beng.fMem[beng.cfc].insSet[beng.insc].addrSlot2.i].type)
        {
            ilhs = beng.sMem[beng.fMem[beng.cfc].insSet[beng.insc].addrSlot2.i].i;
        }
        else if('s' == beng.sMem[beng.fMem[beng.cfc].insSet[beng.insc].addrSlot2.i].type)
        {
            stringFlagS2 = true;
            slhs = beng.sMem[beng.fMem[beng.cfc].insSet[beng.insc].addrSlot2.i].s;
        }
    }
    else if('i' == beng.fMem[beng.cfc].insSet[beng.insc].addrSlot2.type)
    {
        // If its not f,m,or,r its going to be a raw int
        ilhs = beng.fMem[beng.cfc].insSet[beng.insc].addrSlot2.i;
    }
    else if('s' == beng.fMem[beng.cfc].insSet[beng.insc].addrSlot2.type)
    {
        slhs = beng.fMem[beng.cfc].insSet[beng.insc].addrSlot2.s;
        stringFlagS2 = true;
    }

    /*
            GET RHS VAL
    */
    bool stringFlagS3 = false;
    bool floatFlagS3 = false; // Again, assume its not a float

    // Check first slot for existence of floats in s3
    if('f' == beng.fMem[beng.cfc].insSet[beng.insc].addrSlot3.type)
    {
        // Get frhs
        frhs = beng.fMem[beng.cfc].insSet[beng.insc].addrSlot3.f;
        floatFlagS3 = true;
    }   // This checking the contents of a register
    else if ('r' == beng.fMem[beng.cfc].insSet[beng.insc].addrSlot3.type)
    {
        if('f' == beng.rMem[beng.fMem[beng.cfc].insSet[beng.insc].addrSlot3.i].type)
        {
            // Get val from reg
            frhs = beng.rMem[beng.fMem[beng.cfc].insSet[beng.insc].addrSlot3.i].f;
            floatFlagS3 = true;
        }
        else if ('i' == beng.rMem[beng.fMem[beng.cfc].insSet[beng.insc].addrSlot3.i].type)
        {
            irhs = beng.rMem[beng.fMem[beng.cfc].insSet[beng.insc].addrSlot3.i].i;
        }
        else if ('s' == beng.rMem[beng.fMem[beng.cfc].insSet[beng.insc].addrSlot3.i].type)
        {
            stringFlagS3 = true;
            srhs = beng.rMem[beng.fMem[beng.cfc].insSet[beng.insc].addrSlot3.i].s;
        }
    }   // This is checking the contents of a mem slot
    else if ('m' == beng.fMem[beng.cfc].insSet[beng.insc].addrSlot3.type)
    {
        if('f' == beng.sMem[beng.fMem[beng.cfc].insSet[beng.insc].addrSlot3.i].type)
        {
            // Get val from mem
            frhs = beng.sMem[beng.fMem[beng.cfc].insSet[beng.insc].addrSlot3.i].f;
            floatFlagS3 = true;
        }
        else if ('i' == beng.sMem[beng.fMem[beng.cfc].insSet[beng.insc].addrSlot3.i].type)
        {
            irhs = beng.sMem[beng.fMem[beng.cfc].insSet[beng.insc].addrSlot3.i].i;
        }
        else if ('s' == beng.sMem[beng.fMem[beng.cfc].insSet[beng.insc].addrSlot3.i].type)
        {
            srhs = beng.sMem[beng.fMem[beng.cfc].insSet[beng.insc].addrSlot3.i].s;
            stringFlagS3 = true;
        }
    }
    else if('i' == beng.fMem[beng.cfc].insSet[beng.insc].addrSlot3.type)
    {
        irhs = beng.fMem[beng.cfc].insSet[beng.insc].addrSlot3.i;
    }
    else if ('s' == beng.fMem[beng.cfc].insSet[beng.insc].addrSlot3.type)
    {
        srhs = beng.fMem[beng.cfc].insSet[beng.insc].addrSlot3.s;
        stringFlagS3 = true;
    }

    if(stringFlagS2 || stringFlagS3)
    {
        result.type = 's';
    }
    else if (floatFlagS2 || floatFlagS3)
    {
        result.type = 'f';
    }
    else
    {
        result.type = 'i';
    }

    // See which op it is
    switch (beng.fMem[beng.cfc].insSet[beng.insc].operation)
    {

    /*
            Perform Multiplication
    */
    case INSTRUCT_MULT:
    {
        if(BDEBUG)
            std::cout <<"MULT INSTRUCT" << std::endl;

        if(stringFlagS2 || stringFlagS3)
        {
            std::cout << "Error : ALU : Invalud string operation! " << std::endl;
            exit(1);
        }

        // If both are floats
        if(floatFlagS2 && floatFlagS3)
        {
            if(BDEBUG)
                std::cout << "\n\t s2: " << flhs  << " s3: " << frhs << std::endl;
            // Just S2 is float, s3 is int
            result.f = flhs * frhs;
        }
        else if (floatFlagS2)
        {
            if(BDEBUG)
                std::cout << "\n\t s2: " << flhs  << " s3: " << irhs << std::endl;
            // Just S2 is float, s3 is int
            result.f = flhs * (static_cast<double>(irhs));
        }
        else if (floatFlagS3)
        {
            if(BDEBUG)
                std::cout << "\n\t s2: " << ilhs  << " s3: " << frhs << std::endl;
            // Just S2 is float, s3 is int
            result.f = (static_cast<double>(ilhs)) * frhs;
        }
        else
        {
            result.i = ilhs * irhs;
        }
        break;
    }

    /*
            Perform Division
    */
    case INSTRUCT_DIV:
    {
        if(BDEBUG)
            std::cout <<"DIV INSTRUCT" << std::endl;

        if(stringFlagS2 || stringFlagS3)
        {
            std::cout << "Error : ALU : Invalud string operation! " << std::endl;
            exit(1);
        }

        // If both are floats
        if(floatFlagS2 && floatFlagS3)
        {
            if(BDEBUG)
                std::cout << "\n\t s2: " << flhs  << " s3: " << frhs << std::endl;
            // Just S2 is float, s3 is int
            result.f = flhs / frhs;
        }
        else if (floatFlagS2)
        {
            if(BDEBUG)
                std::cout << "\n\t s2: " << flhs  << " s3: " << irhs << std::endl;
            // Just S2 is float, s3 is int
            result.f = flhs / (static_cast<double>(irhs));
        }
        else if (floatFlagS3)
        {
            if(BDEBUG)
                std::cout << "\n\t s2: " << ilhs  << " s3: " << frhs << std::endl;
            // Just S2 is float, s3 is int
            result.f = (static_cast<double>(ilhs)) / frhs;
        }
        else
        {
            result.i = ilhs / irhs;
        }
        break;
    }

    /*
            Perform Addition
    */
    case INSTRUCT_ADD:
    {
        if(BDEBUG)
            std::cout <<"ADD INSTRUCT" << std::endl;

        std::string t = "";

        if(floatFlagS2 && floatFlagS3)
        {
            if(BDEBUG)
                std::cout << "\n\t s2: " << flhs  << " s3: " << frhs << std::endl;
            // Just S2 is float, s3 is int
            result.f = flhs + frhs;
        }
        else if(stringFlagS2 && floatFlagS3)
        {
            std::stringstream ss;
            ss << frhs;
            ss >> t;
            result.s = (slhs + t);
        }
        else if(floatFlagS2 && stringFlagS3)
        {
            std::stringstream ss;
            ss << flhs;
            ss >> t;
            result.s = (t + srhs);
        }
        else if(stringFlagS2 && stringFlagS3)
        {
            result.s = (slhs + srhs);
        }
        else if (floatFlagS2)
        {
            if(BDEBUG)
                std::cout << "\n\t s2: " << flhs  << " s3: " << irhs << std::endl;
            // Just S2 is float, s3 is int
            result.f = flhs + (static_cast<double>(irhs));
        }
        else if (floatFlagS3)
        {
            if(BDEBUG)
                std::cout << "\n\t s2: " << ilhs  << " s3: " << frhs << std::endl;
            // Just S2 is float, s3 is int
            result.f = (static_cast<double>(ilhs)) + frhs;
        }
        else if (stringFlagS2)
        {
            std::stringstream ss;
            ss << irhs;
            ss >> t;
            result.s = (slhs + t);
        }
        else if (stringFlagS3)
        {
            std::stringstream ss;
            ss << ilhs;
            ss >> t;
            result.s = (t + srhs);
        }
        else
        {
            if(BDEBUG)
                std::cout << "\n\t s2: " << ilhs  << " s3: " << irhs << std::endl;
            result.i = ilhs + irhs;
        }
        break;
        
    }

    /*
            Perform Subtraction
    */
    case INSTRUCT_SUB:
    {
        if(BDEBUG)
            std::cout <<"SUB INSTRUCT" << std::endl;

        if(stringFlagS2 || stringFlagS3)
        {
            std::cout << "Error : ALU : Invalud string operation! " << std::endl;
            exit(1);
        }

        // If both are floats
        if(floatFlagS2 && floatFlagS3)
        {
            if(BDEBUG)
                std::cout << "\n\t s2: " << flhs  << " s3: " << frhs << std::endl;
            // Just S2 is float, s3 is int
            result.f = flhs - frhs;
        }
        else if (floatFlagS2)
        {
            if(BDEBUG)
                std::cout << "\n\t s2: " << flhs  << " s3: " << irhs << std::endl;
            // Just S2 is float, s3 is int
            result.f = flhs - (static_cast<double>(irhs));
        }
        else if (floatFlagS3)
        {
            if(BDEBUG)
                std::cout << "\n\t s2: " << ilhs  << " s3: " << frhs << std::endl;
            // Just S2 is float, s3 is int
            result.f = (static_cast<double>(ilhs)) - frhs;
        }
        else
        {
            result.i = ilhs - irhs;
        }
        break;
    }
    default:
        std::cout << "Engine Error: [engine_op_alu()] : Bad operation. HALT" << std::endl;
        exit(1);
        break;
    }

    // Store the result. If r, store in reg. If m, store in mem
    if('r' == beng.fMem[beng.cfc].insSet[beng.insc].addrSlot1.type)
    {
        beng.rMem[beng.fMem[beng.cfc].insSet[beng.insc].addrSlot1.i].type = result.type;
        if('f' == result.type)  // Store in f of mem slot
            beng.rMem[beng.fMem[beng.cfc].insSet[beng.insc].addrSlot1.i].f = result.f;
        else                    // Store in i of mem slot
            beng.rMem[beng.fMem[beng.cfc].insSet[beng.insc].addrSlot1.i].i = result.i;
    }
    else
    {
        beng.sMem[beng.fMem[beng.cfc].insSet[beng.insc].addrSlot1.i].type = result.type;
        if('f' == result.type)  // Store in f of mem slot
            beng.sMem[beng.fMem[beng.cfc].insSet[beng.insc].addrSlot1.i].f = result.f;
        else                    // Store in i of mem slot
            beng.sMem[beng.fMem[beng.cfc].insSet[beng.insc].addrSlot1.i].i = result.i;
    }

    if(BDEBUG)
    {
        std::cout << "\n\tALU: RESULT TYPE: " << result.type << " RESULT DATA: ";
        if(result.type == 'f')
            std::cout << result.f;
        else if(result.type == 'i')
            std::cout << result.i;
        else
            std::cout << result.s;
            
        std::cout << std::endl;
    }

    // Find out where we are storing the result, and send it in.
    if('r' == beng.fMem[beng.cfc].insSet[beng.insc].addrSlot1.type)
    {
        engine_reg_add(beng, result, beng.fMem[beng.cfc].insSet[beng.insc].addrSlot1.i);
    }
    else if ('m' == beng.fMem[beng.cfc].insSet[beng.insc].addrSlot1.type)
    {
        engine_mem_add(beng, result, beng.fMem[beng.cfc].insSet[beng.insc].addrSlot1.i);
    }


    ++beng.insc;
}

/*
            PERFORM JMP - Complete
*/
void engine_op_jmp(eng &beng)
{
    beng.insc = beng.fMem[beng.cfc].insSet[beng.insc].addrSlot1.i+1;
}

/*
            PERFORM BRANCH - Complete
*/
void engine_op_branch(eng &beng)
{
    // Left and right hand side of equations
    long long ilhs, irhs;
    double flhs, frhs;
    std::string slhs, srhs;

    // Figure out if floats are around. . .
    bool floatFlagS2 = false; // Assume not a float
    
    bool stringFlagS2 = false;
    
    

    /*
            GET RHS VAL
    */
    
    if(BDEBUG)
    {
        std::cout << "RHS VALUE : " << beng.fMem[beng.cfc].insSet[beng.insc].addrSlot2.type <<std::endl;
    }
    
    
    // Check first slot for existence of floats in s2
    if('f' == beng.fMem[beng.cfc].insSet[beng.insc].addrSlot2.type)
    {
        // Set flag and get lhs for mathing later
        frhs = beng.fMem[beng.cfc].insSet[beng.insc].addrSlot2.f;
        floatFlagS2 = true;
    }   // This checking the contents of a register
    else if ('r' == beng.fMem[beng.cfc].insSet[beng.insc].addrSlot2.type)
    {
        if('f' == beng.rMem[beng.fMem[beng.cfc].insSet[beng.insc].addrSlot2.i].type)
        {
            // Grab the lhs from the reg
            frhs = beng.rMem[beng.fMem[beng.cfc].insSet[beng.insc].addrSlot2.i].f;
            floatFlagS2 = true;
        }
        else if ('i' == beng.rMem[beng.fMem[beng.cfc].insSet[beng.insc].addrSlot2.i].type)
        {
            irhs = beng.rMem[beng.fMem[beng.cfc].insSet[beng.insc].addrSlot2.i].i;
        }
        else if('s' == beng.rMem[beng.fMem[beng.cfc].insSet[beng.insc].addrSlot2.i].type)
        {
            srhs = beng.rMem[beng.fMem[beng.cfc].insSet[beng.insc].addrSlot2.i].s;
            stringFlagS2 = true;
        }
        
    }   // This is checking the contents of a mem slot
    else if ('m' == beng.fMem[beng.cfc].insSet[beng.insc].addrSlot2.type)
    {
        if('f' == beng.sMem[beng.fMem[beng.cfc].insSet[beng.insc].addrSlot2.i].type)
        {
            // Grab the lhs from the mem slot
            frhs = beng.sMem[beng.fMem[beng.cfc].insSet[beng.insc].addrSlot2.i].f;
            floatFlagS2 = true;
        }
        else if('i' == beng.sMem[beng.fMem[beng.cfc].insSet[beng.insc].addrSlot2.i].type)
        {
            irhs = beng.sMem[beng.fMem[beng.cfc].insSet[beng.insc].addrSlot2.i].i;
        }
        else if('s' == beng.sMem[beng.fMem[beng.cfc].insSet[beng.insc].addrSlot2.i].type)
        {
            srhs = beng.sMem[beng.fMem[beng.cfc].insSet[beng.insc].addrSlot2.i].s;
            stringFlagS2 = true;
        }
    }
    else if('i' == beng.fMem[beng.cfc].insSet[beng.insc].addrSlot2.type)
    {
        // If its not f,m,or,r its going to be a raw int
        irhs = beng.fMem[beng.cfc].insSet[beng.insc].addrSlot2.i;
    }
    else if('s' == beng.fMem[beng.cfc].insSet[beng.insc].addrSlot2.type)
    {
        srhs = beng.fMem[beng.cfc].insSet[beng.insc].addrSlot2.s;
        stringFlagS2 = true;
    }

    /*
            GET LHS VAL
    */
    
    if(BDEBUG)
    {
        std::cout << "LHS VALUE : " << beng.fMem[beng.cfc].insSet[beng.insc].addrSlot1.type <<std::endl;
    }
    
    
    
    bool floatFlagS1 = false; // Again, assume its not a float
    
    bool stringFlagS1 = false;

    // Check first slot for existence of floats in s1
    if('f' == beng.fMem[beng.cfc].insSet[beng.insc].addrSlot1.type)
    {
        // Get frhs
        flhs = beng.fMem[beng.cfc].insSet[beng.insc].addrSlot1.f;
        floatFlagS1 = true;
    }   // This checking the contents of a register
    else if ('r' == beng.fMem[beng.cfc].insSet[beng.insc].addrSlot1.type)
    {
        if('f' == beng.rMem[beng.fMem[beng.cfc].insSet[beng.insc].addrSlot1.i].type)
        {
            // Get val from reg
            flhs = beng.rMem[beng.fMem[beng.cfc].insSet[beng.insc].addrSlot1.i].f;
            floatFlagS1 = true;
        }
        else if ('i' == beng.rMem[beng.fMem[beng.cfc].insSet[beng.insc].addrSlot1.i].type)
        {
            ilhs = beng.rMem[beng.fMem[beng.cfc].insSet[beng.insc].addrSlot1.i].i;
        }
        else if('s' == beng.rMem[beng.fMem[beng.cfc].insSet[beng.insc].addrSlot1.i].type)
        {
            slhs = beng.rMem[beng.fMem[beng.cfc].insSet[beng.insc].addrSlot1.i].s;
            stringFlagS1 = true;
        }
    }   // This is checking the contents of a mem slot
    else if ('m' == beng.fMem[beng.cfc].insSet[beng.insc].addrSlot1.type)
    {
        if('f' == beng.sMem[beng.fMem[beng.cfc].insSet[beng.insc].addrSlot1.i].type)
        {
            // Get val from mem
            flhs = beng.sMem[beng.fMem[beng.cfc].insSet[beng.insc].addrSlot1.i].f;
            floatFlagS1 = true;
        }
        else if ('i' == beng.sMem[beng.fMem[beng.cfc].insSet[beng.insc].addrSlot1.i].type)
        {
            ilhs = beng.sMem[beng.fMem[beng.cfc].insSet[beng.insc].addrSlot1.i].i;
        }
        else if('s' == beng.sMem[beng.fMem[beng.cfc].insSet[beng.insc].addrSlot1.i].type)
        {
            slhs = beng.sMem[beng.fMem[beng.cfc].insSet[beng.insc].addrSlot1.i].s;
            stringFlagS1 = true;
        }
    }
    else if('i' == beng.fMem[beng.cfc].insSet[beng.insc].addrSlot1.type)
    {
        ilhs = beng.fMem[beng.cfc].insSet[beng.insc].addrSlot1.i;
    }
    else if('s' == beng.fMem[beng.cfc].insSet[beng.insc].addrSlot1.type)
    {
        slhs = beng.fMem[beng.cfc].insSet[beng.insc].addrSlot1.s;
        stringFlagS1 = true;
    }
    
    /*
            Check which type of branch we will be looking at
    */
    bool doBranch = false;
    if(1.0 == beng.fMem[beng.cfc].insSet[beng.insc].addrSlot3.f)
    {
        // BLE
        if(BDEBUG)
        {
            std::cout << "\nBLE\n";
        }
        
        if(floatFlagS1 && floatFlagS2)
        {
            doBranch = (flhs <= frhs);
        }
        else if(stringFlagS1 && floatFlagS2)
        {
            std::cout << "Engine Error: Can not comp string and float object in branch" << std::endl;
            exit(1);
        }
        else if(floatFlagS1 && stringFlagS2)
        {
            std::cout << "Engine Error: Can not comp string and float object in branch" << std::endl;
            exit(1);
        }
        else if(stringFlagS1 && stringFlagS2)
        {
            doBranch = (slhs <= srhs);
        }
        else if (floatFlagS1)
        {
            doBranch = ( flhs <= irhs);
        }
        else if (floatFlagS2)
        {
            doBranch = ( ilhs <= frhs );
        }
        else if (stringFlagS1)
        {
            std::cout << "Engine Error: Can not comp string and int object in branch" << std::endl;
            exit(1);
        }
        else if (stringFlagS2)
        {
            std::cout << "Engine Error: Can not comp string and int object in branch" << std::endl;
            exit(1);
        }
        else
        {
            doBranch = (ilhs <= irhs);
        }
    }
    else if (2.0 == beng.fMem[beng.cfc].insSet[beng.insc].addrSlot3.f)
    {
        // BNE
        if(BDEBUG)
        {
            std::cout << "\nBNE\n";
        }
        
        if(floatFlagS1 && floatFlagS2)
        {
            doBranch = (flhs != frhs);
        }
        else if(stringFlagS1 && floatFlagS2)
        {
            std::cout << "Engine Error: Can not comp string and float object in branch" << std::endl;
            exit(1);
        }
        else if(floatFlagS1 && stringFlagS2)
        {
            std::cout << "Engine Error: Can not comp string and float object in branch" << std::endl;
            exit(1);
        }
        else if(stringFlagS1 && stringFlagS2)
        {
            doBranch = (slhs != srhs);
        }
        else if (floatFlagS1)
        {
            doBranch = ( flhs != irhs);
        }
        else if (floatFlagS2)
        {
            doBranch = ( ilhs != frhs );
        }
        else if (stringFlagS1)
        {
            std::cout << "Engine Error: Can not comp string and int object in branch" << std::endl;
            exit(1);
        }
        else if (stringFlagS2)
        {
            std::cout << "Engine Error: Can not comp string and int object in branch" << std::endl;
            exit(1);
        }
        else
        {
            doBranch = (ilhs != irhs);
        }
    }
    else if (3.0 == beng.fMem[beng.cfc].insSet[beng.insc].addrSlot3.f)
    {
        // BGT
        if(BDEBUG)
        {
            std::cout << "\nBGT\n";
        }
        
        if(floatFlagS1 && floatFlagS2)
        {
            doBranch = (flhs > frhs);
        }
        else if(stringFlagS1 && floatFlagS2)
        {
            std::cout << "Engine Error: Can not comp string and float object in branch" << std::endl;
            exit(1);
        }
        else if(floatFlagS1 && stringFlagS2)
        {
            std::cout << "Engine Error: Can not comp string and float object in branch" << std::endl;
            exit(1);
        }
        else if(stringFlagS1 && stringFlagS2)
        {
            doBranch = (slhs > srhs);
        }
        else if (floatFlagS1)
        {
            doBranch = ( flhs > irhs);
        }
        else if (floatFlagS2)
        {
            doBranch = ( ilhs > frhs );
        }
        else if (stringFlagS1)
        {
            std::cout << "Engine Error: Can not comp string and int object in branch" << std::endl;
            exit(1);
        }
        else if (stringFlagS2)
        {
            std::cout << "Engine Error: Can not comp string and int object in branch" << std::endl;
            exit(1);
        }
        else
        {
            doBranch = (ilhs > irhs);
        }
    }
    else if (4.0 == beng.fMem[beng.cfc].insSet[beng.insc].addrSlot3.f)
    {
        // BLT
        if(BDEBUG)
        {
            std::cout << "\nBLT\n";
        }
        
        if(floatFlagS1 && floatFlagS2)
        {
            doBranch = (flhs < frhs);
        }
        else if(stringFlagS1 && floatFlagS2)
        {
            std::cout << "Engine Error: Can not comp string and float object in branch" << std::endl;
            exit(1);
        }
        else if(floatFlagS1 && stringFlagS2)
        {
            std::cout << "Engine Error: Can not comp string and float object in branch" << std::endl;
            exit(1);
        }
        else if(stringFlagS1 && stringFlagS2)
        {
            doBranch = (slhs < srhs);
        }
        else if (floatFlagS1)
        {
            doBranch = ( flhs < irhs);
        }
        else if (floatFlagS2)
        {
            doBranch = ( ilhs < frhs );
        }
        else if (stringFlagS1)
        {
            std::cout << "Engine Error: Can not comp string and int object in branch" << std::endl;
            exit(1);
        }
        else if (stringFlagS2)
        {
            std::cout << "Engine Error: Can not comp string and int object in branch" << std::endl;
            exit(1);
        }
        else
        {
            doBranch = (ilhs < irhs);
        }
    }
    else if (5.0 == beng.fMem[beng.cfc].insSet[beng.insc].addrSlot3.f)
    {
        // BEQ
        if(BDEBUG)
        {
            std::cout << "\nBEQ\n";
        }
        
        if(floatFlagS1 && floatFlagS2)
        {
            doBranch = (flhs == frhs);
        }
        else if(stringFlagS1 && floatFlagS2)
        {
            std::cout << "Engine Error: Can not comp string and float object in branch" << std::endl;
            exit(1);
        }
        else if(floatFlagS1 && stringFlagS2)
        {
            std::cout << "Engine Error: Can not comp string and float object in branch" << std::endl;
            exit(1);
        }
        else if(stringFlagS1 && stringFlagS2)
        {
            doBranch = (slhs == srhs);
        }
        else if (floatFlagS1)
        {
            doBranch = ( flhs == irhs);
        }
        else if (floatFlagS2)
        {
            doBranch = ( ilhs == frhs );
        }
        else if (stringFlagS1)
        {
            std::cout << "Engine Error: Can not comp string and int object in branch" << std::endl;
            exit(1);
        }
        else if (stringFlagS2)
        {
            std::cout << "Engine Error: Can not comp string and int object in branch" << std::endl;
            exit(1);
        }
        else
        {
            doBranch = (ilhs == irhs);
        }
    }
    else if (6.0 == beng.fMem[beng.cfc].insSet[beng.insc].addrSlot3.f)
    {
        // BGE
        if(BDEBUG)
        {
            std::cout << "\nBGE\n";
        }
        
        if(floatFlagS1 && floatFlagS2)
        {
            doBranch = (flhs >= frhs);
        }
        else if(stringFlagS1 && floatFlagS2)
        {
            std::cout << "Engine Error: Can not comp string and float object in branch---12" << std::endl;
            exit(1);
        }
        else if(floatFlagS1 && stringFlagS2)
        {
            std::cout << "Engine Error: Can not comp string and float object in branch---22" << std::endl;
            exit(1);
        }
        else if(stringFlagS1 && stringFlagS2)
        {
            doBranch = (slhs >= srhs);
        }
        else if (floatFlagS1)
        {
            doBranch = ( flhs >= irhs) ;
        }
        else if (floatFlagS2)
        {
            doBranch = ( ilhs >= frhs );
        }
        else if (stringFlagS1)
        {
            std::cout << "Engine Error: Can not comp string and int object in branch---1" << std::endl;
            exit(1);
        }
        else if (stringFlagS2)
        {
            std::cout << "Engine Error: Can not comp string and int object in branch---2" << std::endl;
            exit(1);
        }
        else
        {
            doBranch = (ilhs >= irhs);
        }
    }
    else
    {
        // ERROR!
        std::cout << "Engine Error : [engine_op_jmp(beng)] : Unknown Branch Instruction HALT" << std::endl;
        exit(1);
    }

    // If we do branch, move the instruction counter to the instruction of the label
    if(doBranch)
    {
        beng.insc = beng.fMem[beng.cfc].insSet[beng.insc].addrSlot3.i+1;
    }
    else
    {
        ++beng.insc;
    }
}

/*
            PERFORM PRINT - Complete
*/
void engine_op_print(eng &beng)
{
    switch(beng.fMem[beng.cfc].insSet[beng.insc].addrSlot1.type)
    {
    case 'r':
    {
        switch(beng.rMem[beng.fMem[beng.cfc].insSet[beng.insc].addrSlot1.i].type)
        {
        case 'i':
            std::cout << beng.rMem[beng.fMem[beng.cfc].insSet[beng.insc].addrSlot1.i].i;
            break;
        case 'f':
            std::cout << beng.rMem[beng.fMem[beng.cfc].insSet[beng.insc].addrSlot1.i].f;
            break;
        case 's':
        {
            // Go through the string, and handle special charachters \n \t etc
            for(unsigned i = 0; i < beng.rMem[beng.fMem[beng.cfc].insSet[beng.insc].addrSlot1.i].s.length(); ++i)
            {
                if('\\' == beng.rMem[beng.fMem[beng.cfc].insSet[beng.insc].addrSlot1.i].s[i])
                {
                    if( (i+1) < beng.rMem[beng.fMem[beng.cfc].insSet[beng.insc].addrSlot1.i].s.length() )
                    {
                        if('n' == beng.rMem[beng.fMem[beng.cfc].insSet[beng.insc].addrSlot1.i].s[i+1])
                        {
                            std::cout << std::endl;
                            i++;
                        }
                        else if('t' == beng.rMem[beng.fMem[beng.cfc].insSet[beng.insc].addrSlot1.i].s[i+1])
                        {
                            std::cout << "\t";
                            i++;
                        }
                    }
                }
                else
                {
                    std::cout << beng.rMem[beng.fMem[beng.cfc].insSet[beng.insc].addrSlot1.i].s[i];
                }
            }
            break;
        }
        case 'b':
            std::cout << beng.rMem[beng.fMem[beng.cfc].insSet[beng.insc].addrSlot1.i].b;
            break;
        }

        break;
    }
    // IF ITS A MEM SLOT, DETERMINE TYPE, AND DISP
    case 'm':
    {
        switch(beng.sMem[beng.fMem[beng.cfc].insSet[beng.insc].addrSlot1.i].type)
        {
        case 'i':
            std::cout << beng.sMem[beng.fMem[beng.cfc].insSet[beng.insc].addrSlot1.i].i;
            break;
        case 'f':
            std::cout << beng.sMem[beng.fMem[beng.cfc].insSet[beng.insc].addrSlot1.i].f;
            break;
        case 's':
        {
            // Go through the string, and handle special charachters \n \t etc
            for(unsigned i = 0; i < beng.sMem[beng.fMem[beng.cfc].insSet[beng.insc].addrSlot1.i].s.length(); ++i)
            {
                if('\\' == beng.sMem[beng.fMem[beng.cfc].insSet[beng.insc].addrSlot1.i].s[i])
                {
                    if( (i+1) < beng.sMem[beng.fMem[beng.cfc].insSet[beng.insc].addrSlot1.i].s.length() )
                    {
                        if('n' == beng.sMem[beng.fMem[beng.cfc].insSet[beng.insc].addrSlot1.i].s[i+1])
                        {
                            std::cout << std::endl;
                            i++;
                        }
                        else if('t' == beng.sMem[beng.fMem[beng.cfc].insSet[beng.insc].addrSlot1.i].s[i+1])
                        {
                            std::cout << "\t";
                            i++;
                        }
                    }
                }
                else
                {

                    std::cout << beng.sMem[beng.fMem[beng.cfc].insSet[beng.insc].addrSlot1.i].s[i];
                }
            }
            break;
        }
        case 'b':
            std::cout << beng.sMem[beng.fMem[beng.cfc].insSet[beng.insc].addrSlot1.i].b;
            break;
        }

        break;
    }
    // IF ITS NOT A REG OR A MEM, JUST SHOW THE DATA
    case 'i':
        std::cout << beng.fMem[beng.cfc].insSet[beng.insc].addrSlot1.i;
        break;
    case 'f':
        std::cout << beng.fMem[beng.cfc].insSet[beng.insc].addrSlot1.f;
        break;
    case 's':
    {
        // Go through the string, and handle special charachters \n \t etc
        for(unsigned i = 0; i < beng.fMem[beng.cfc].insSet[beng.insc].addrSlot1.s.length(); ++i)
        {
            if('\\' == beng.fMem[beng.cfc].insSet[beng.insc].addrSlot1.s[i])
            {
                if( (i+1) < beng.fMem[beng.cfc].insSet[beng.insc].addrSlot1.s.length() )
                {
                    if('n' == beng.fMem[beng.cfc].insSet[beng.insc].addrSlot1.s[i+1])
                    {
                        std::cout << std::endl;
                        ++i;
                    }
                    else if('t' == beng.fMem[beng.cfc].insSet[beng.insc].addrSlot1.s[i+1])
                    {
                        std::cout << "\t";
                        ++i;
                    }
                }
            }
            else
            {

                std::cout << beng.fMem[beng.cfc].insSet[beng.insc].addrSlot1.s[i];
            }
        }
        break;
    }
    case 'b':
        std::cout << beng.fMem[beng.cfc].insSet[beng.insc].addrSlot1.b;
        break;
    }
    ++beng.insc;
}

/*
            PERFORM PRINT - Complete
*/
void eng_op_read(eng &beng)
{
    mslt result;

    if(BDEBUG)
        std::cout << "\n\n\tNEED TO TEST THIS - ENGINE - OP_READ" << std::endl;
    
    // Get the users input
    std::getline(std::cin, result.s) ;

    bool num = true;
    bool hasP = false;
    for(unsigned i = 0; i < result.s.length(); ++i)
    {
        if('.' != result.s[i] && ' ' != result.s[i] && '-' != result.s[0])
        {
            if(!std::isdigit(result.s[i]))
                    num = false;
        }
        else if('.' == result.s[i])
        {
            hasP = true;
        }
    }

    // Chek the data we just went through to determine type
    if(hasP && num)
    {
        result.type = 'f';
        result.f = stod(result.s);
    }
    else if(num)
    {
        result.type = 'i';
        if(result.s.length() > 19)
        {
            std::cout << "Error: Number entered out of range of 'long long' (9223372036854775807)\n";
            std::cout << "\tNumber was set to 0\n";
            result.i = 0;
        }
        else
        {
            result.i = stoll(result.s);
        }
    }
    else
    {
        result.type = 's';
    }

    // Read into a register or mem
    if ('r' == beng.fMem[beng.cfc].insSet[beng.insc].addrSlot1.type)
    {
        engine_reg_add(beng, result, beng.fMem[beng.cfc].insSet[beng.insc].addrSlot1.i);
    }
    else if ('m' == beng.fMem[beng.cfc].insSet[beng.insc].addrSlot1.type)
    {
        engine_mem_add(beng, result, beng.fMem[beng.cfc].insSet[beng.insc].addrSlot1.i);
    }
}

/*
            PERFORM FILE OUT - Complete
*/
void eng_op_file(eng &beng)
{
    std::string fileName, data;
    // Get the file name, either from the instruction, or from the storage area
    if(beng.fMem[beng.cfc].insSet[beng.insc].addrSlot1.type == 's')
    {
        fileName = beng.fMem[beng.cfc].insSet[beng.insc].addrSlot1.s;
    }
    else if(beng.fMem[beng.cfc].insSet[beng.insc].addrSlot1.type == 'r')
    {
        fileName = beng.rMem[beng.fMem[beng.cfc].insSet[beng.insc].addrSlot1.i].s;
    }
    else if(beng.fMem[beng.cfc].insSet[beng.insc].addrSlot1.type == 'm')
    {
        fileName = beng.sMem[beng.fMem[beng.cfc].insSet[beng.insc].addrSlot1.i].s;
    }

    // Get the data
    if(beng.fMem[beng.cfc].insSet[beng.insc].addrSlot3.type == 's')
    {
        data = beng.fMem[beng.cfc].insSet[beng.insc].addrSlot3.s;
    }
    else if(beng.fMem[beng.cfc].insSet[beng.insc].addrSlot3.type == 'r')
    {
        data = beng.rMem[beng.fMem[beng.cfc].insSet[beng.insc].addrSlot3.i].s;
    }
    else if(beng.fMem[beng.cfc].insSet[beng.insc].addrSlot3.type == 'm')
    {
        data = beng.sMem[beng.fMem[beng.cfc].insSet[beng.insc].addrSlot3.i].s;
    }

    // Create file object
    std::fstream ofile;

    // Check file output mode, and act accordingly
    if(beng.fMem[beng.cfc].insSet[beng.insc].addrSlot2.b == true)
    {
        // Do a write
        ofile.open(fileName, std::ios::out);
        if(!ofile.is_open())
        {
            std::cout << "Engine Error : [eng_op_file()] : "
                      << "unable to write-to : " << fileName << std::endl;
            exit(1);
        }
        ofile << data;
    }
    else if(beng.fMem[beng.cfc].insSet[beng.insc].addrSlot2.b == false)
    {
        // Do an append
        ofile.open(fileName, std::ios::app);
        if(!ofile.is_open())
        {
            std::cout << "Engine Error : [eng_op_file()] : "
                      << "unable to append-to : " << fileName << std::endl;
            exit(1);
        }
        ofile << data;
    }
}

/*
            PERFORM MOV - Complete
*/
void engine_op_mov(eng &beng)
{
    // Determine what the destination is, and copy the data
    mslt temp;
    switch(beng.fMem[beng.cfc].insSet[beng.insc].addrSlot1.type)
    {
    case 'r':
    {
        // If destination is reg -> See where we get data from.
        switch(beng.fMem[beng.cfc].insSet[beng.insc].addrSlot2.type)
        {
        case 'r':
            // Copying register data to a register, so grab the item from the source register
            // Then put in register slot of the source
            temp = engine_reg_get(beng, beng.fMem[beng.cfc].insSet[beng.insc].addrSlot2.i);
            engine_reg_add(beng, temp, beng.fMem[beng.cfc].insSet[beng.insc].addrSlot1.i);
            break;
        case 'm':
            // Copying memory data to a register, so grab the item from the source mem slot
            // Then put in register slot of the source
            temp = engine_mem_get(beng, beng.fMem[beng.cfc].insSet[beng.insc].addrSlot2.i);
            engine_reg_add(beng, temp, beng.fMem[beng.cfc].insSet[beng.insc].addrSlot1.i);
            break;
        default:
            // If its not a register, or mem slot, just add everything
            temp.i = beng.fMem[beng.cfc].insSet[beng.insc].addrSlot2.i;
            temp.s = beng.fMem[beng.cfc].insSet[beng.insc].addrSlot2.s;
            temp.f = beng.fMem[beng.cfc].insSet[beng.insc].addrSlot2.f;
            temp.b = beng.fMem[beng.cfc].insSet[beng.insc].addrSlot2.b;
            temp.type = beng.fMem[beng.cfc].insSet[beng.insc].addrSlot2.type;
            engine_reg_add(beng, temp, beng.fMem[beng.cfc].insSet[beng.insc].addrSlot1.i);
            break;
        }
        break;
    }
    case 'm':
    {
        // If destination is reg -> See where we get data from.
        switch(beng.fMem[beng.cfc].insSet[beng.insc].addrSlot2.type)
        {
        case 'r':
            // Copying register data to a register, so grab the item from the source register
            // Then put in register slot of the source
            temp = engine_reg_get(beng, beng.fMem[beng.cfc].insSet[beng.insc].addrSlot2.i);
            engine_mem_add(beng, temp, beng.fMem[beng.cfc].insSet[beng.insc].addrSlot1.i);
            break;
        case 'm':
            // Copying memory data to a register, so grab the item from the source mem slot
            // Then put in register slot of the source
            temp = engine_mem_get(beng, beng.fMem[beng.cfc].insSet[beng.insc].addrSlot2.i);
            engine_mem_add(beng, temp, beng.fMem[beng.cfc].insSet[beng.insc].addrSlot1.i);
            break;
        default:
            // If its not a register, or mem slot, just add everything
            temp.i = beng.fMem[beng.cfc].insSet[beng.insc].addrSlot2.i;
            temp.s = beng.fMem[beng.cfc].insSet[beng.insc].addrSlot2.s;
            temp.f = beng.fMem[beng.cfc].insSet[beng.insc].addrSlot2.f;
            temp.b = beng.fMem[beng.cfc].insSet[beng.insc].addrSlot2.b;
            temp.type = beng.fMem[beng.cfc].insSet[beng.insc].addrSlot2.type;
            engine_mem_add(beng, temp, beng.fMem[beng.cfc].insSet[beng.insc].addrSlot1.i);
            break;
        }
        break;
    }
    default:
        std::cout << "Engine Error : [engine_op_mov()] : Invalid Mov Destination" << std::endl;
        exit(1);
        break;
    }
    // Inc the insctruction counter
    ++beng.insc;
}

/*
            PROCESS AN INSTRUCTION
*/
void engine_process_instruct(eng &beng)
{
    if(BDEBUG)
    {
        std::cout << "The current frame is \t\t: " << beng.cfc << std::endl
                  << "The current instruct is \t: " << beng.insc
                  << std::endl;
    }

    // Grab the instruction from engine's insc location and see what the
    // operation is.
    switch (beng.fMem[beng.cfc].insSet[beng.insc].operation)
    {
    case INSTRUCT_MULT:
        engine_op_alu(beng);
        break;
    case INSTRUCT_DIV:
        engine_op_alu(beng);
        break;
    case INSTRUCT_ADD:
        engine_op_alu(beng);
        break;
    case INSTRUCT_SUB:
        engine_op_alu(beng);        
        break;
    case INSTRUCT_JMP:
        // move bengin.insc to the jmp location+1
        engine_op_jmp(beng);
        break;
    case INSTRUCT_CALL:
        // Load the item onto the stack
        engine_stack_load(beng, beng.fMem[beng.cfc].insSet[beng.insc].addrSlot1.i, true);
        break;
    case INSTRUCT_MOV:
        // Copy mem data into reg or mem slot
        engine_op_mov(beng);
        break;
    case INSTRUCT_BLE:
        // do comparison - branches to label location+1
        engine_op_branch(beng);
        break;
    case INSTRUCT_BNE:
        // do comparison - branches to label location+1
        engine_op_branch(beng);
        break;
    case INSTRUCT_BGT:
        // do comparison - branches to label location+1
        engine_op_branch(beng);
        break;
    case INSTRUCT_BLT:
        // do comparison - branches to label location+1
        engine_op_branch(beng);
        break;
    case INSTRUCT_BEQ:
        engine_op_branch(beng);
        break;
    case INSTRUCT_BGE:
        engine_op_branch(beng);
        break;
    case INSTRUCT_P:
        engine_op_print(beng);
        break;
    case INSTRUCT_LABEL:
        // If the thing is a label, we can ignore it. Its just a place holder
        ++beng.insc;
        break;
    case INSTRUCT_READ:
        // Make sure that this newly added instruction doesn't mess up labels
        eng_op_read(beng);
        ++beng.insc;
        break;
    case INSTRUCT_FILE:
         eng_op_file(beng);
        ++beng.insc;
        break;
    default:
        std::cout << "Engine Error: [engine_process_instruct("
                  << beng.fMem[beng.cfc].insSet[beng.insc].operation
                  << ")] : Unknown Instruction HALT" << std::endl;
        exit(1);
        break;
    }

    if(beng.insc < beng.fMem[beng.cfc].insSet.size())
        engine_process_instruct(beng);

    // At this point the program will return to the while
    // loop in process to check if the stack is empty yet or not.
}

/*
        BEGINE EXECUTION
        Called by main to begin execution of program


*/
void process(eng &beng, unsigned entryPoint)
{
    if(BDEBUG)
        std::cout << "\n\n\n\n" << "ACCESSED: blang::process(&beng, "
                  << entryPoint <<  ");\n\n\t--- BLANG ENGINE ---\n\n"
                  << "Entry Function : " << entryPoint << std::endl;

    // Add the entry function onto the stack
    engine_stack_load(beng, entryPoint, 0);

    // Initialize reg slots.
    for(unsigned i =0; i < ENG_REG_COUNT; ++i)
    {
        beng.rMem[i].type = 'n';
    }

    // Reset engine counters just in case
    beng.cfc = entryPoint;
    beng.insc = 0;
    beng.funcRet.type = 'n';

    // Initialize memory - Uses funcRet solely for the purpose of not making another object...
    beng.sMem.push_back(beng.funcRet);

    // While there are items on the stack
    while(!beng.fStack.empty())
    {
        // Process the current instruction
        // If the current instruction is the last in the set, pop the frame
        if(beng.insc < beng.fMem[beng.cfc].insSet.size())
            engine_process_instruct(beng);
        else
            engine_stack_pop(beng);
        // this works fine. Could be a reaaallly bad time.
    }
}


} // End name space

#endif // ENGINE
