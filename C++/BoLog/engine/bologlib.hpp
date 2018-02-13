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

#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <iterator>
#include <algorithm>


#include "bolog.hpp"



#ifndef bologlib_h_HEADER_GUARD
#define bologlib_h_HEADER_GUARD

# define BOLOG_FILE_HELP_PAGE "syntax_help.txt"
namespace blg {
    
    class lib{
    public:
        
        lib();
        lib(bool interface);
        lib(int argc, const char * argv);
        ~lib();
        
        // True / False response
        bool submit(std::string query);
        
        // Return data
        std::string query(std::string query);
        
        
        void userHelp();
        void userSession();
        
    private:
        
        int lc;
        bool file_eating_mode;
        bool user_session_mode;
        bool bolog_global_run_flag;
        
        
        bolog bl;
        
        // Helpers
        VS splitString(std::string s, char token);
        bool containsItem(std::string s, std::string item);
        std::string rtrim(std::string s, char del);
        std::string ltrim(std::string s, char del);
        std::string trim(std::string s, char del);
        std::string stripChar(char c, std::string str);
        
        
        void loadFile(std::string);
        BRE handleReturnItems(BRE item, int lc);
        BRE process(std::string query, bool flag);
        
        
        
    };
    
    
    
#endif /* bologlib_h */
}
