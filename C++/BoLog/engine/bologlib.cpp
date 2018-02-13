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

#include "bologlib.hpp"

namespace blg {
    
    // Initial startup
    lib::lib(){
        lc = 0;
        file_eating_mode = false;
        user_session_mode = false;
        bolog_global_run_flag = true;
    }
    
    // Start a user session
    lib::lib(bool session){
        if(session){
            lc = 0;
            file_eating_mode = false;
            user_session_mode = false;
            bolog_global_run_flag = true;
            userSession();
        }
    }
    
    // Load files into the lib
    lib::lib(int argc, const char * argv){
        lc = 0;
        file_eating_mode = true;
        user_session_mode = false;
        bolog_global_run_flag = true;
        std::string line;
        for(int i = 1; i < argc; i++){
            loadFile(&argv[i]);
        }
    }
    
    lib::~lib(){
        
    }
    
    // Submit something / check for yes/no
    bool lib::submit(std::string query){
        // If its not a good reponse, we don't care
        
        
        BRE resp = process(query, true);
        if(resp.code != RETURN_D && resp.code != RETURN_G){
            return false;
        }
        return true;
    }
    
    // Query for a response
    std::string lib::query(std::string query){
        return process(query, true).functionReturnMessage;
    }
    
    // Show help file
    void lib::userHelp(){
        std::fstream f(BOLOG_FILE_HELP_PAGE);
        if(f.is_open()){
            std::string line;
            while(!f.eof()){
                std::getline(f, line);
                std::cout << line << std::endl;
            }
        } else {
            std::cout << "[Error] -> Unable to locate help page ["
            << BOLOG_FILE_HELP_PAGE << "]" << std::endl << std::endl;
        }
        f.close();
    }
    
    // Handle a user session
    void lib::userSession() {
        int lc = 0;
        std::string line;
        user_session_mode = true;
        while(bolog_global_run_flag){
            std::getline(std::cin,  line);
            if(line == "exit" || line == "q"){
                bolog_global_run_flag = false;
            } else if (line == "help"){
                userHelp();
            } else {
                if(line.length() > 0){
                    process(line, lc);
                    lc++;
                }
            }
        }
    }

    // Private
    
    // Helpers
    
    // Split a string by the token, and return a VS of data
    VS lib::splitString(std::string s, char token){
        VS temp;
        std::string obj;
        std::istringstream ss(s);
        while(std::getline(ss, obj, token)){
            temp.push_back(obj);
        }
        return temp;
    }

    // Check a string to check the existence of something (ignores stringed info)
    bool lib::containsItem(std::string s, std::string item)
    {
        bool inStr = false;
        std::string curr = "";
        for(unsigned i=0;i<s.length();i++){
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
    std::string lib::rtrim(std::string s, char del){
        size_t endpos = s.find_last_not_of(del);
        return (endpos == std::string::npos) ? "" : s.substr(0, endpos+1);
    }
    
    // Trim left side of the string
    std::string lib::ltrim(std::string s, char del){
        size_t startpos = s.find_first_not_of(del);
        return (startpos == std::string::npos) ? "" : s.substr(startpos);
    }
    
    // Calls left and right trim on the input
    std::string lib::trim(std::string s, char del){
        return rtrim(ltrim(s, del), del);
    }
    
    // Remove something from a string
    std::string lib::stripChar(char c, std::string str){
        std::string t = "";
        for(int i = 0; i < str.length(); i++){
            if(str[i] != c)
                t += str[i];
        }
        return t;
    }
    
    // Load a file into the engine
    void lib::loadFile(std::string path){
        int lc = 0;
        std::fstream file(path);
        std::string line;
        if(file.is_open()){
            lc = 0;
            while(!file.eof() && bolog_global_run_flag){
                std::getline(file, line);
                stripChar('\n', line);
                if(line.length() > 0){
                    BRE t = process(line, lc);
                    if(file_eating_mode && t.code == RETURN_F){
                        bolog_global_run_flag  = false;
                    }
                }
                lc++;
            }
        } else {
            BRE t("File Error",
                  "Unable to open file! ", RETURN_F);
            handleReturnItems(t, lc);
        }
        file.close();
    }
    
    // Handle the return items from the bolog engine
    BRE lib::handleReturnItems(BRE item, int lc){
        BRE ret("[hri]","NA", RETURN_G);
        switch(item.code){
            case RETURN_G:
                break;
            case RETURN_W:
                
                if(user_session_mode){
                    std::cout << "("<< lc <<")[WARNING]:" << item.functionReturning
                    << std::endl << "\t\t-> " << item.functionReturnMessage << std::endl;
                    return ret;
                }
                ret.code = RETURN_F;
                return ret;
                break;
            case RETURN_F:
                // Don't exit fail if in user mode. Only when in file mode
                if(user_session_mode){
                    std::cout << "(" << lc << ")[FAIL]:" << item.functionReturning
                    << std::endl << "\t\t-> " << item.functionReturnMessage << std::endl;
                    
                    std::cout << "--> Line not accepted !\n...\n";

                }
                ret.code = RETURN_F;
                return ret;
                break;
            case RETURN_Y:
                if(user_session_mode){
                    std::cout << "< YES >" << std::endl;    // A yes response to inquiry
                }
                ret.code = RETURN_G;
                ret.functionReturnMessage = "Y";
                return ret;
                break;
            case RETURN_N:
                if(user_session_mode){
                    std::cout << "< NO >" << std::endl;     // A no response to inquiry
                }
                ret.functionReturnMessage = "N";
                ret.code = RETURN_F;
                return ret;
                break;
            case RETURN_D:
                if(user_session_mode){
                    // Engine sent data back from a function!
                    std::cout << item.functionReturning << "\n["
                    << item.functionReturnMessage << "]" << std::endl;
                }
                ret.functionReturnMessage = item.functionReturnMessage;
                return ret;
                
                break;
            default:
                bolog_global_run_flag = false;
                std::cout << "[ERROR]->Default accessed in paser <handleReturnItems>"
                << std::endl;
                ret.code = RETURN_F;
                break;
        }
        return ret;
    }

    
    // Process a line
    BRE lib::process(std::string line, bool flag) {
        
        BRE ret("[process]", "NA", RETURN_G);
    
        // Convert everything to lowercase
        std::transform(line.begin(), line.end(), line.begin(), ::tolower);
        
        // Handle syntactical errors
        if(line.length() <= 1){
            BRE t("Parser Syntax Error", "Invalid length", RETURN_F);
            return handleReturnItems(t,lc);
        }
        
        switch(line[0]) {
            case DEFINE_FACT: {                                 // Send FACT information to bolog
                line = stripChar(DEFINE_FACT, line);
                VS data = splitString(line, ' ');
                
                data.erase(data.begin());
                
                // Handle silly input, and single input
                if(data.size() == 2){
                    if (data[0] == data[1]){
                        BRE t("Syntax Error",
                              "Can't have fact group, and object be the same", RETURN_F);
                        return handleReturnItems(t, lc);
                    } else {
                        return handleReturnItems(bl.createFact(data), lc);
                    }
                } else if (data.size() > 2) {
                    // Multi fact input ( DEFINE_FACT fruit apple banana grape )
                    VS temp;
                    for(VSI i = data.begin()+1; i != data.end(); ++i){
                        temp.clear();
                        temp.push_back(data[0]);                    // Grab the category
                        temp.push_back((*i));
                        if(bl.createFact(temp).code == RETURN_F){
                            ret.code = RETURN_F;
                            return ret;
                        }
                    }
                    ret.code = RETURN_G;
                    return handleReturnItems(ret, lc);
                } else {
                    BRE t("Syntax Error",
                          "Invalid number of items in fact.", RETURN_F);
                    return handleReturnItems(t, lc);
                }
                break;
            }
                
            case DEFINE_RULE: {
                // Strip the ID
                line = stripChar(DEFINE_RULE, line);
                VS data = splitString(line, ' ');
                data.erase(data.begin());
                
                // Each rule should have no less than 3 pieces
                if(data.size() < 3){
                    BRE t("Syntax Error",
                          "Invalid number of items in rule.", RETURN_F);
                    return handleReturnItems(t, lc);
                } else {
                    // Go through everything listed as associated to the object via the token
                    // and send them piece by piece to the engine [ object token item ] x n
                    VS temp;
                    for(VSI i = data.begin()+2; i != data.end(); ++i){
                        temp.clear();
                        temp.push_back(data[0]);                    // Grab the object
                        temp.push_back(data[1]);                    // Grab the token
                        temp.push_back((*i));
                        BRE t = bl.createRule(temp);
                        if(t.code == RETURN_F){
                            ret.code = RETURN_F;
                            return ret;
                        }
                    }
                    ret.code = RETURN_G;
                    return handleReturnItems(ret, lc);
                }
                break;
            }
                
            case DEFINE_QUES: {
                line = stripChar(DEFINE_QUES, line);
                VS data = splitString(line, ' ');
                data.erase(data.begin());
                
                if(data.size() == 2){
                    // Fact query
                    return handleReturnItems(bl.factInquiry(data, ""), lc);
                } else if (data.size() == 3) {
                    // Rule query
                    return handleReturnItems(bl.ruleInquiry(data, ""), lc);
                } else if (data[0] == data[1]) {
                    BRE t("[Default]",
                          "A thing is always its self.", RETURN_G);
                    return handleReturnItems(t, lc);
                } else {
                    BRE t("Syntax Error",
                          "Invalid number of items in inquiry.", RETURN_F);
                    return handleReturnItems(t, lc);
                }
                break;
            }
                
            case DEFINE_SPLL: {
                line = stripChar(DEFINE_SPLL, line);
                VS data = splitString(line, ' ');
                data.erase(data.begin());
                if(data.size() != 1){
                    BRE t("Syntax Error",
                          "Invalid number of items in fact dump.", RETURN_F);
                    return handleReturnItems(t, lc);
                } else {
                    return handleReturnItems(bl.retFactData(data[0]), lc);
                }
                break;
            }
                
            case DEFINE_COMM: {
                // Ignore comments
                break;
            }
                
            case DEFINE_SRUL: {
                line = stripChar(DEFINE_SRUL, line);
                VS data = splitString(line, ' ');
                data.erase(data.begin());
                if(data.size() != 1){
                    BRE t("Syntax Error",
                          "Invalid number of items in rule dump.", RETURN_F);
                    return handleReturnItems(t, lc);
                } else {
                    return handleReturnItems(bl.retRuleTokens(data[0]), lc);
                }
                break;
            }
                
            case DEFINE_TOKE: {
                line = stripChar(DEFINE_TOKE, line);
                VS data = splitString(line, ' ');
                data.erase(data.begin());
                if(data.size() != 2){
                    BRE t("Syntax Error",
                          "Invalid number of items in token dump.", RETURN_F);
                    return handleReturnItems(t, lc);
                } else {
                    return handleReturnItems(bl.retTokenMembers(data[0], data[1]), lc);
                }
                break;
            }
                
            case DEFINE_FILE: {
                line = stripChar(DEFINE_FILE, line);
                line = stripChar(' ', line);
                if(line.length() <= 1){
                    BRE t("Syntax Error",
                          "File name must be >= 1, and contain no spaces", RETURN_F);
                    return handleReturnItems(t, lc);
                }
                // Load the file into the engine
                loadFile(line);
                break;
            }
                
            case DEFINE_DELE: {
                line = stripChar(DEFINE_DELE, line);
                VS data = splitString(line, ' ');
                data.erase(data.begin());
                
                if(data.size() <= 1){
                    BRE t("Syntax Error",
                          "Deletion syntax incorrect", RETURN_F);
                    return handleReturnItems(t, lc);
                }
                
                /*
                 Check specifics
                 */
                if(data.size() == 2 ||
                   data.size() == 3 ||
                   data.size() == 4)
                {
                    return handleReturnItems(bl.performDeletion(data), lc);
                    
                } else {
                    BRE t("Syntax Error",
                          ("Invalid deletion request."), RETURN_F);
                    return handleReturnItems(t, lc);
                }
                break;
            }
                
            default:
                std::cout << "Syntax Error. Invalid init token on line [" << lc << "]"
                << std::endl;
                break;
        }
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        return ret;
    }
}
