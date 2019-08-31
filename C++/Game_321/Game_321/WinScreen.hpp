//
//  WinScreen.hpp
//  Game_321
//
//  Created by Josh Bosley on 11/23/16.
//  Copyright © 2016 Josh Bosley. All rights reserved.
//

#ifndef WinScreen_hpp
#define WinScreen_hpp

#include <iostream>
#include "common.h"
#include "biped.hpp"
#include "SDLAssist.hpp"

class WinCondition
{
public:
    WinCondition();
    
private:
    SDLAssist _sdla;
    
    void mainLoop();
};


#endif /* WinScreen_hpp */
