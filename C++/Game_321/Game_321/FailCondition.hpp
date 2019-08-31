//
//  FailCondition.hpp
//  Game_321
//
//  Created by Josh Bosley on 11/23/16.
//  Copyright © 2016 Josh Bosley. All rights reserved.
//

#ifndef FailCondition_hpp
#define FailCondition_hpp


#include <iostream>
#include "common.h"
#include "biped.hpp"
#include "SDLAssist.hpp"

class FailCondition
{
public:
    FailCondition();
    
private:
    SDLAssist _sdla;
    
    void mainLoop();
};


#endif /* FailCondition_hpp */
