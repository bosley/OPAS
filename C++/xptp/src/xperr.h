/*
 * The MIT License
 *
 * Copyright 2018 Josh A. Bosley.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

/* 
 * File:   xperr.h
 * Author: Josh A. Bosley
 *
 * Created on April 2, 2018, 11:49 PM
 */

#ifndef XPERR_H
#define XPERR_H

#include <iostream>

#define XPERR_ERROR 0
#define XPERR_WARNING 1

#define DIABLED_WHILE_ACTIVE 100

namespace xptp{
    class xperr{
    public:
        int code;
        std::string message;
        int etype;
        xperr(int e, int c, std::string m ){
            this->etype = e;
            this->code = c;
            this->message = m;
        }
        friend std::ostream& operator<<(std::ostream& os, const xperr& xp)  
        {  
            if(xp.etype == XPERR_ERROR)
                os << "XPTP Error [" << xp.code << "] " << xp.message;
            else if (xp.etype == XPERR_WARNING)
                os << "XPTP Warning [" << xp.code << "] " << xp.message;
                
            return os;  
        }  
    };
    
}

#endif /* XPERR_H */

