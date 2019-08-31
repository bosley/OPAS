//
//  GameLevelThree.hpp
//  Game_321
//
//  Created by Josh Bosley on 11/4/16.
//  Copyright © 2016 Josh Bosley. All rights reserved.
//

#ifndef GameLevelThree_hpp
#define GameLevelThree_hpp

#include <stdio.h>
#include <iostream>
#include <vector>
#include "common.h"
#include "biped.hpp"
#include "SDLAssist.hpp"
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include <algorithm>

class GameLevelThree
{
    public :
    GameLevelThree(humanoid &human);
    
private:
    SDLAssist _sdla;
    
    biped bipedController;
    
    SDL_Color textColor;
    
    enum currentState {learnMove, learnHealth, learnInventory, transition};
    
    currentState levelState;
    bool finalEvent;
    bool finalDialogComplete;
    unsigned finalEventStage;
    
    std::vector<humanoid> bipeds;
    
    bool doHandleSpriteMovement;
    bool playerCollided;
    int numberOfCollisions;
    char lastDetectedCollision;
    int playerLastX, playerLastY;
    bool pylon1, pylon2, pylon3, pylon4;
    unsigned pStage1, pStage2, pStage3, pStage4;
    bool pylonWatch; // Checks for event of all pylons being destroyed
    unsigned pylonDestCount;
    
    
    
    SDL_Rect attackInFacingDirection(humanoid &attacker);                           // Retrieve hitbox for melee area of effect (AOE)
    
    void performBounceBack(humanoid &h);
    int checkForItemObjectColliding(SDL_Rect item);
    
    void updateHeartDisplay(humanoid &human);
    
    void updateBibeds(humanoid &human);
    
    void levelMainLoop(humanoid &human);
    
};

#endif /* GameLevelThree_hpp */
