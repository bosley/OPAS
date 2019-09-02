//
//  GameLevelTutorial.hpp
//  Game_321
//
//  Created by Josh Bosley on 10/12/16.
//  Copyright © 2016 Josh Bosley. All rights reserved.
//

#ifndef GameLevelTutorial_hpp
#define GameLevelTutorial_hpp

#include <stdio.h>
#include <iostream>
#include "SDLAssist.hpp"

class GameLevelTutorial
{
public :
    GameLevelTutorial();
    
private:
    SDLAssist _sdla;
    
    void levelMainLoop();
};

#endif /* GameLevelTutorial_hpp */
