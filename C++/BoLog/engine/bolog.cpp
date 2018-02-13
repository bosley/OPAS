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

#include "bolog.hpp"

namespace blg {

bolog::bolog(){
    
}

bolog::~bolog(){
    
}

/*

        Create a new fact
 
    fact [ category, item ]
 
 */
BRE bolog::createFact(VS fact){
    BRE ret("[createFact]", "NONE", RETURN_G);
    
#ifdef BOLOG_DEBUG_FACTS
    debugMessage("Creating fact:", fact);
#endif

    // If fact exists
    if(factExists(fact[0])){
     
        // If the member exists, ignore the request
        if(factMemberExists(fact[0], fact[1]))
        {
            ret.functionReturnMessage = "Non-Unique object <ignored>";
            ret.code = RETURN_W;
            return ret;
        }
        
        // Fact exists, but the member doesn't so it gets added
        _facts[fact[0]].push_back(fact[1]);
        
    } else {
        
        // Fact didn't exist, do create it and its member
        VS n; n.push_back(fact[1]);
        _facts.insert(
                      std::make_pair(fact[0], n)
                      );
    }
    return ret;
}

/*
 
        Create a new rule
 
    rule [ <object | group> join_token <object | group> ]
 
 */
BRE bolog::createRule(VS rule){
#ifdef BOLOG_DEBUG_RULES
    std::cout << "Creating rule ... " << std::endl;
    for(VSI i = rule.begin(); i != rule.end(); ++i){
        std::cout << "\t" << (*i) << std::endl;
    }
#endif
    BRE ret("[createRule]", "NONE", RETURN_G);
    
    // Check to see if rule is in _rules
    // If not, add it, return
    RMAPI finder = _rules.find(rule[0]);
    
    // Rule doesn't exist, need to add it
    if(finder == _rules.end()){
        
#ifdef BOLOG_DEBUG_RULES
        std::cout << "Add a rule !\n";
#endif
        _rules[rule[0]][rule[1]].push_back(rule[2]);
    } else {
        // Rule does exist.
#ifdef BOLOG_DEBUG_RULES
        std::cout << "Rule for [" << rule[0] << "] exists! \n";
#endif
        // Does the token exist within the rule?
        UMCI tokenFinder = _rules[rule[0]].find(rule[1]);
        if (tokenFinder != _rules[rule[0]].end()){
            // Middle token found.
            // Check to see if rhs is in list. if not add it
            
#ifdef BOLOG_DEBUG_RULES
            std::cout << "Found token [" << rule[1] << "]" << std::endl;
#endif
            for(VSI i = _rules[rule[0]][rule[1]].begin();
                i != _rules[rule[0]][rule[1]].end(); ++i){
                
                // Ensure it's not already in the list
                if((*i) == rule[2]){
                    ret.code = RETURN_W;
                    ret.functionReturnMessage =
                    ("Token [" + rule[1] + "] for [" + rule[0]
                     + "] already has memeber [" + (*i)+ "]");
                    return ret;
                }
#ifdef BOLOG_DEBUG_RULES
                std::cout << "\t-> token [" << rule[1] << "] contains member ["
                << (*i) << "]" << std::endl;
#endif
            }
#ifdef BOLOG_DEBUG_RULES
            std::cout << "\t-> token [" << rule[1] << "] added member ["
            << rule[2] << "]" << std::endl;
#endif
            // If it wasn't in the list, it'll get here
            _rules[rule[0]][rule[1]].push_back(rule[2]);
        } else {
            // No middle term, need to add it and whatever is on rhs
#ifdef BOLOG_DEBUG_RULES
            std::cout << "Didn't find token [" << rule[1] << "]" << std::endl;
            std::cout << "Token [" << rule[1] << "] added, with member ["
            << rule[1] << "]" << std::endl;
#endif
            _rules[rule[0]][rule[1]].push_back(rule[2]);
        }
    }
    return ret;
}

/*
 
        Perform a fact inquiry
 
    inquiry [ object, group ]
 
 
    - This can be improved upon for better handling of 
      large fact sets
 
    - May want to include a cache to stop from searching
      previously searched objects
 
 */
BRE bolog::factInquiry(VS inq, std::string last){
    
    
    
    
    
#ifdef BOLOG_DEBUG_INQUI
    std::cout << "inq[0]= " << inq[0]
    << " | inq[1]= " << inq[1]
    << " | last= " << last << std::endl;
#endif
    
    // Assume it won't find anything
    BRE ret("[factInquiry]", "NONE", RETURN_N);
    
    // See if the group is a fact in the system
    UMCI finder = _facts.find(inq[1]);
    if(finder == _facts.end()){
        // If it isn't found return RETURN_N
        return ret;
    }
 
    // Check to see if the object is in the group we selected
    for(VSI i = (*finder).second.begin();
        i != (*finder).second.end(); i++ ){
        if ((*i) == inq[0]){
            ret.code = RETURN_Y;     // If it is found, return out
            }
    }
    
    // Perform recursion on every element in the original's list
    // to find relations
    for(VSI i = (*finder).second.begin();
        i != (*finder).second.end(); i++ ){
        if (last != (*i)){
            // Create a vector for recursing on
            VS n; n.push_back(inq[0]);  // Add the obj we're searching for
            n.push_back((*i));          // Add the item in the list
            
            if(factInquiry(n, inq[1]).code == RETURN_Y){
                ret.code = RETURN_Y;
           }
        }
    }
    return ret;
}

/*
 
        Perform a rule inquiry
 
 */
BRE bolog::ruleInquiry(VS inq, std::string last){
#ifdef BOLOG_DEBUG_INQUI
    std::cout << "inq[0]= " << inq[0]
    << " | inq[1]= " << inq[1]
    << " | inq[2]= " << inq[2]
    << " | last= " << last << std::endl;
#endif
    BRE ret("[ruleInquiry]", "NONE", RETURN_N);
    
    // Check if rule exists
    RMAPI finder = _rules.find(inq[0]);
    if(finder == _rules.end()){
        std::cout << "rule [" << inq[0] << "] doesn't exist\n";
        ret.functionReturnMessage = ("rule [" + inq[0] + "] doesn't exist.");
        return ret;
    }
    
    // Check that token exists within rule
    UMCI tokenFinder = _rules[inq[0]].find(inq[1]);
    if (tokenFinder == _rules[inq[0]].end()){
        std::cout << "token[" << inq[1] << "] doens't exist for rule ["
        << inq[0] << "]" << std::endl;
        
        ret.functionReturnMessage =
        ("token [" + inq[1] + "] doesn't exist for rule [" + inq[0] + "]");
        return ret;
    }
    
#ifdef BOLOG_DEBUG_INQUI
    std::cout << "[" << inq[0] << "] has token [" << inq[1] << "]" << std::endl;
#endif
    
    // Go through list belonging to that token within the rule
    
    for(VSI i = _rules[inq[0]][inq[1]].begin();
        i != _rules[inq[0]][inq[1]].end(); ++i ){
        
        // The inquired thing existed directly to this token
        if((*i) == inq[2]){
            
            
#ifdef BOLOG_DEBUG_INQUI
            std::cout << "\t --> "
            << inq[2] << " is inside token " << inq[1] << std::endl;
#endif
            
            ret.code = RETURN_Y;
            return ret;
        }
        
#ifdef BOLOG_DEBUG_INQUI
        std::cout << "\t --> " << (*i) << std::endl;
#endif
    }
    return ret;
}

/*
 
        Show fact data
        - Takes in a string to find a fact, returns all 
          members as a csv in return type under 
            " functionReturnMessage "
 
 */
BRE bolog::retFactData(std::string fact){
    
#ifdef BOLOG_DEBUG_SPILL_FACT
    std::cout << "Spill fact : " << fact << std::endl;
#endif
    
    BRE ret("[retFactData]", "NA", RETURN_D);  // Mark this as a data returning function
    
    UMCI finder = _facts.find(fact);
    if(finder == _facts.end()){
        ret.code = RETURN_W;
        ret.functionReturnMessage = ("No fact known as [" + fact + "]");
        return ret;
    }
    
    VS members;
#ifdef BOLOG_DEBUG_SPILL_FACT
    std::cout << "[Showing members for : " << fact << "]" << std::endl;
#endif
    for(VSI i = (*finder).second.begin();
        i != (*finder).second.end(); ++i ){
#ifdef BOLOG_DEBUG_SPILL_FACT
        std::cout << "\t--> " << (*i) << std::endl;
#endif
        members.push_back(*i);
    }
    
    // Return the list of belongings so they can be used if need-be
    ret.functionReturnMessage = vectorToCSV(members);
    
    // If theyre aren't any there, "NA" will still be the message
    return ret;
}

/*
 
        Show rule tokens
 
 */
BRE bolog::retRuleTokens(std::string rule){
#ifdef BOLOG_DEBUG_SPILL_TOKENS
    std::cout << "Spill tokens" << std::endl;
#endif
    BRE ret("[retRuleTokens]", "NA", RETURN_D);
    
    RMAPI finder = _rules.find(rule);
    if(finder == _rules.end()){
        ret.code = RETURN_W;
        ret.functionReturnMessage = ("No rule known as [" + rule + "]");
        return ret;
    }
    
    VS members;
#ifdef BOLOG_DEBUG_SPILL_TOKENS
    std::cout << "[Showing tokens for : " << rule << "]" << std::endl;
#endif
    for(UMCI i = _rules[rule].begin(); i != _rules[rule].end(); ++i ){
#ifdef BOLOG_DEBUG_SPILL_TOKENS
        std::cout << "\t--> " << (*i).first << std::endl;
#endif
        members.push_back((*i).first);
        
    }
    
    // Return the list of belongings so they can be used if need-be
    ret.functionReturnMessage = vectorToCSV(members);
    return ret;
}

/*
 
        Show rule's token members
 
 */
BRE bolog::retTokenMembers(std::string rule, std::string token){
#ifdef BOLOG_DEBUG_SPILL_TOKEN_MEMBERS
    std::cout << "Spill token members" << std::endl;
#endif
    BRE ret("[showRuleTokens]", "NA", RETURN_D);
    
    // Ensure its a rule
    RMAPI finder = _rules.find(rule);
    if(finder == _rules.end()){
        ret.code = RETURN_W;
        ret.functionReturnMessage = ("No rule known as [" + rule + "]");
        return ret;
    }
    
    // Ensure its a token
    // Check that token exists within rule
    UMCI tokenFinder = _rules[rule].find(token);
    if (tokenFinder == _rules[rule].end()){
        std::cout << "token[" << token << "] doens't exist for rule ["
        << rule << "]" << std::endl;
        
        ret.functionReturnMessage =
        ("token [" + token + "] doesn't exist for rule [" + rule + "]");
        return ret;
    }
    
#ifdef BOLOG_DEBUG_SPILL_TOKEN_MEMBERS
    std::cout << "[Showing members for : "
    << token << "] within rule [" << rule << "]" << std::endl;
    
#endif
    
    VS members;
    // Go through each token member
    for(VSI i = _rules[rule][token].begin();
        i != _rules[rule][token].end(); ++i ){
#ifdef BOLOG_DEBUG_SPILL_TOKEN_MEMBERS
        std::cout << "\t--> " << (*i) << std::endl;
#endif
        members.push_back((*i));
    }
    // Return the list of belongings so they can be used if need-be
    ret.functionReturnMessage = vectorToCSV(members);
    return ret;
}

/*
 
        Perform a deletion of a fact, rule, token or any member
 
 */
BRE bolog::performDeletion(VS data){
#ifdef BOLOG_DEBUG_DELETION
    std::cout << "Deletion request : " << std::endl;
    for(VSI i = data.begin(); i != data.end(); ++i){
        std::cout << "\t -->" << (*i) << std::endl;
    }
#endif
    
    BRE ret("[performDeletion]", "NA", RETURN_G);
    
    // Figure out which we are deleting!
    // ( Fact, Fact-Member, Rule, Rule-Token, or Rule-Token-Membet )
    switch(data[0][0]){
            /*
======================  RULE DELETION  ======================
             */
        case DEFINE_RULE:{
            
            if(data.size() == 2){
#ifdef BOLOG_DEBUG_DELETION
                std::cout << "Delete rule [" << data[1] << "]" << std::endl;
#endif
                
// Delete rule
// ----------------------------------------
                if(ruleExists(data[1])){
                    _rules[data[1]].clear();
                    _rules.erase(data[1]);
                } else {
                    ret.code = RETURN_W;
                    ret.functionReturnMessage = ("Rule [" + data[1] + "] does not exist <ignored>");
                    return ret;
                }
// ----------------------------------------
            } else if (data.size() == 3) {
#ifdef BOLOG_DEBUG_DELETION
                std::cout << "Delete rule [" << data[1] << "]'s token [" << data[2] << "]" << std::endl;
#endif

// Delete rule token
// ----------------------------------------
                if(ruleExists(data[1])){
                    if(tokenExistsWithinRule(data[1], data[2])){
                        _rules[data[1]].erase(data[2]);
                    } else {
                        ret.code = RETURN_W;
                        ret.functionReturnMessage = (
                            "Rule [" + data[1] +
                            "] does not have token [" + data[2] + "]  <ignored>");
                        return ret;
                    }
                } else {
                    ret.code = RETURN_W;
                    ret.functionReturnMessage = ("Rule [" + data[1] + "] does not exist <ignored>");
                    return ret;
                }
                
// ----------------------------------------
            } else if (data.size() == 4) {
#ifdef BOLOG_DEBUG_DELETION
                std::cout << "Delete rule ["
                << data[1] << "]'s token [" << data[2] << "] member ["
                << data[3] << "]" << std::endl;
#endif
                
// Delete rule's token member
// ----------------------------------------
                if(ruleExists(data[1])){
                    if(tokenExistsWithinRule(data[1], data[2])){
                        if(memberExistsWithinExistingToken(data[1], data[2], data[3])){
    
                            // Delete the member
                            _rules[data[1]][data[2]].erase(
                                                      std::remove(
                                                                  _rules[data[1]][data[2]].begin(),
                                                                  _rules[data[1]][data[2]].end(), data[3]),
                                                      _rules[data[1]][data[2]].end()
                                                      );
                        } else {
                            ret.code = RETURN_W;
                            ret.functionReturnMessage = (
                                "Rule [" + data[1] +
                                "]'s token [" + data[2] +"] doesn't have token [" +
                                data[3] + "] <ignored>");
                            return ret;
                        }
                    } else {
                        ret.code = RETURN_W;
                        ret.functionReturnMessage = (
                                                     "Rule [" + data[1] +
                                                     "] does not have token [" + data[2] + "]  <ignored>");
                        return ret;
                    }
                } else {
                    ret.code = RETURN_W;
                    ret.functionReturnMessage = ("Rule [" + data[1] + "] does not exist <ignored>");
                    return ret;
                }
// ----------------------------------------
            } else {
                // Technically handled by parser
                ret.code = RETURN_F;
                ret.functionReturnMessage = "Invalid deletion request";
                return ret;
            }
            break;
        }
        /*
======================  FACT DELETION  ======================
         */
        case DEFINE_FACT:{
            
            if(data.size() == 2){
#ifdef BOLOG_DEBUG_DELETION
                std::cout << "Delete fact [" << data[1] << "]" << std::endl;
#endif
// Delete fact
// ----------------------------------------
                if(factExists(data[1])){
                    _facts.erase(data[1]);
                } else {
                    ret.code = RETURN_F;
                    ret.functionReturnMessage =
                    ("Fact [" + data[1] + "] doesn't exist <ignored>");
                    return ret;
                }
// ----------------------------------------
            } else if (data.size() == 3) {
#ifdef BOLOG_DEBUG_DELETION
                std::cout << "Delete fact [" << data[1]
                << "]'s member [" << data[2] << "]" << std::endl;
#endif
// Delete fact member
// ----------------------------------------
                if(factExists(data[1])){
                    if(factMemberExists(data[1],
                                        data[2])){
                        // Delete the item
                        _facts[data[1]].erase(
                                              std::remove(
                                                          _facts[data[1]].begin(),
                                                          _facts[data[1]].end(), data[2]),
                                              _facts[data[1]].end()
                                              );
                    } else {
                        ret.code = RETURN_F;
                        ret.functionReturnMessage =
                        ("Fact [" + data[1] + "] doesn't have member [" + data[2] + "] <ignored>");
                        return ret;
                    }
                } else {
                    ret.code = RETURN_F;
                    ret.functionReturnMessage =
                    ("Fact [" + data[1] + "] doesn't exist <ignored>");
                    return ret;
                }
// ----------------------------------------
            } else {
                // Technically handled by parser
                ret.code = RETURN_F;
                ret.functionReturnMessage = "Invalid deletion request";
                return ret;
            }
            break;
        }
        default:{
            ret.code = RETURN_F;
            ret.functionReturnMessage = "Unknown deletion init token";
            return ret;
            break;
        }
    }
    return ret;
}



/*          PRIVATE FUNCTIONS       */



/*
 
        Takes in a VS, and turns it into a CSV string
 
 */
std::string bolog::vectorToCSV(VS members){
    std::string ret = "NA";
    if(!members.empty()){
        std::ostringstream oss;
        std::copy(members.begin(), members.end()-1,
                  std::ostream_iterator<std::string>(oss, ","));
        oss << members.back();
        ret = oss.str();  // Data to return
    }
    return ret;
}

/*
 
        Check if a given fact exists
 
 */
bool bolog::factExists(std::string fact){
    UMCI finder = _facts.find(fact);
    if(finder == _facts.end()){
        return false;
    }
    return true;
}

/*
 
        Check if member exists within fact
 
 */
bool bolog::factMemberExists(std::string fact, std::string member){
    // Ensure its unique to this fact
    for(VSI i = _facts[fact].begin();
        i != _facts[fact].end(); i++)
    {
        if((*i) == member){
            return true;
        }
    }
    return false;
}

/*
 
        Check if a given rule exists
 
 */
bool bolog::ruleExists(std::string rule){
    RMAPI finder = _rules.find(rule);
    if(finder == _rules.end()){
        return false;
    }
    return true;
}

/*
 
        Check if a given token exists within rule
 
 */
bool bolog::tokenExistsWithinRule(std::string rule, std::string token){
    // This assumes that the given rule already exists!
    UMCI tokenFinder = _rules[rule].find(token);
    if (tokenFinder == _rules[rule].end()){
        return false;
    }
    return true;
}

/*
 
        Check if a member exits within a token of a rule
 
 */
bool bolog::memberExistsWithinExistingToken(std::string rule,
                                            std::string token, std::string member){
    VSI i = _rules[rule][token].begin();
    for(; i != _rules[rule][token].end(); ++ i){
        if((*i) == member){
            return true;
        }
    }
    return false;
}

/*
 
        Debug information out
 
 */
#if defined(BOLOG_DEBUG_FACTS) || defined(BOLOG_DEBUG_RULES) || defined(BOLOG_DEBUG_INQUI)
void bolog::debugMessage(std::string message, VS data){
    std::cout << "[DEBUG] : " << message << std::endl;
    for(VSI i = data.begin(); i != data.end(); ++i){
        std::cout << "\t-> " << (*i) << std::endl;
    }
}
#endif
}
