/*
 The MIT License (MIT)
 Copyright (c) 2017 Josh A. Bosley
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

#ifndef bolog_hpp
#define bolog_hpp
// Displays information about fact making
//#define BOLOG_DEBUG_FACTS 1

// Displays information about rule making
//#define BOLOG_DEBUG_RULES 1

// Displays information about inquiry processing
//#define BOLOG_DEBUG_INQUI 1

// Displays information about spilling fact info
//#define BOLOG_DEBUG_SPILL_FACT 1

// Displays information about spilling rule info
//#define BOLOG_DEBUG_SPILL_TOKENS 1

// Displays information about spilling token info
//#define BOLOG_DEBUG_SPILL_TOKEN_MEMBERS 1

// Displays information about deleting an item
//#define BOLOG_DEBUG_DELETION 1

#include <string>
#include <vector>
#include <sstream>
#include <iterator>
#include <iostream>
#include <unordered_map>

// Syntax IDS
#define DEFINE_FACT '-'         // Define fact
#define DEFINE_RULE '.'         // Define rule
#define DEFINE_QUES '?'         // Inquiries
#define DEFINE_COMM '#'         // Comments
#define DEFINE_SPLL '$'         // Spill fact
#define DEFINE_SRUL '='         // Spill rule tokens
#define DEFINE_TOKE '~'         // Spill rule's token members
#define DEFINE_FILE '@'         // Handled by parser, not bolog engine
#define DEFINE_DELE '!'         // Request a deletion

// Return types
#define RETURN_G ' '            // Success
#define RETURN_W '-'            // Warning
#define RETURN_F '_'            // Failure
#define RETURN_Y 'Y'            // Yes repsonse
#define RETURN_N 'N'            // No response
#define RETURN_D 'D'            // Returning data!

namespace blg {
    
struct bolog_return_error{
    /*
            Return object to inform parser that an issue occured
     */
    std::string functionReturning;
    std::string functionReturnMessage;
    char code;
    // A constructor for ease of use
    bolog_return_error(std::string func, std::string msg, char c){
        this->functionReturning = func;
        this->functionReturnMessage = msg;
        this->code = c;
    }
};
typedef struct bolog_return_error BRE;          // Make the name shorter

typedef std::vector<std::string> VS;             // Vector of strings
typedef std::vector<std::string>::iterator VSI;  // VS Iterator
typedef std::unordered_map<std::string, VS> UM;  // Hashed storage
typedef std::unordered_map<std::string, VS>::iterator UMCI;// Unordered map iterator
typedef std::pair<std::string, VS> UME;          // Element of storage

// Hashed rule stroage - Rules toke
typedef std::unordered_map<std::string, UM> RMAP;
typedef std::unordered_map<std::string, UM>::iterator RMAPI;

/*
 
        The bolog engine
 
 */
class bolog{
    
public:
    bolog();
    ~bolog();
    
    BRE createFact(VS fact);
    BRE createRule(VS rule);
    
    // Inquiry, and a string to identify last thing searched
    // this is to handle recursive looping segfault potential
    BRE factInquiry(VS inq, std::string last);
    BRE ruleInquiry(VS inq, std::string last);
    
    // Retrieve the respectie datas via BRE's functionReturnMessage (CSV)
    BRE retFactData(std::string fact);
    BRE retRuleTokens(std::string rule);
    BRE retTokenMembers(std::string rule, std::string token);
    
    // Remove an item (fact, rule or otherwise)
    BRE performDeletion(VS data);
    
private:
    
    std::string vectorToCSV(VS vec);
    
    // Helper functions
    bool factExists(std::string fact);
    bool factMemberExists(std::string fact, std::string member);
    bool ruleExists(std::string rule);
    bool tokenExistsWithinRule(std::string rule, std::string token);
    bool memberExistsWithinExistingToken(std::string rule,
                                         std::string token, std::string member);
    
    UM _facts;                  // Unordered map of vector strings
    RMAP _rules;                // Unordered map of bolog_rules
    
    // Just a debugging function - Leave out when compiled for real use
#if defined(BOLOG_DEBUG_FACTS) || defined(BOLOG_DEBUG_RULES) || defined(BOLOG_DEBUG_INQUI)
    void debugMessage(std::string message, VS data);
#endif
};



#endif /* bolog_hpp */
    
}
