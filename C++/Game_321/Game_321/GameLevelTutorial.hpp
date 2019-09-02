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
#include "Scene.hpp"
#include "common.h"
#include "biped.hpp"
#include "SDLAssist.hpp"

class GameLevelTutorial
{
public :
    GameLevelTutorial(humanoid &human);
    
private:
    SDLAssist _sdla;
    
    biped bipedController;
    
    SDL_Color textColor;
    
    enum currentState {learnMove, learnHealth, learnInventory, transition};
    
    currentState levelState;
    
    bool doHandleSpriteMovement;
    
    void levelMainLoop(humanoid &human);
};

#endif /* GameLevelTutorial_hpp */
