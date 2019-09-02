//
//  GameLevelOne.hpp
//  Game_321
//
//  Created by Josh Bosley on 10/12/16.
//  Copyright © 2016 Josh Bosley. All rights reserved.
//

#ifndef GameLevelOne_hpp
#define GameLevelOne_hpp

#include <stdio.h>
#include <iostream>
#include "common.h"
#include "biped.hpp"
#include "SDLAssist.hpp"

class GameLevelOne
{
public :
    GameLevelOne(humanoid &human);                                                  // Init, call main loop, clean
    
private:
    SDLAssist _sdla;                                                                // SDL Assist class
    
    biped bipedController;                                                          // Manipulate bipeds
    
    SDL_Color textColor;                                                            // text color
    
    enum currentState {walkIn, introDialog, exitBoss, learnFight, firstCombat, transOut};     // Possible level states
    
    currentState levelState;                                                        // Level state controller
    
    humanoid alienGuardOne, alienGuardTwo, boss, traitor;                           // Aliens on this level
    
    char itemThatCollidedWithPlayer;
    
    bool doHandleSpriteMovement;                                                    // Toggle ability to move
    
    bool checkForItemObjectColliding(SDL_Rect item);                                // Test all level items
    
    bool checkForPlayerHumanoidColliding(SDL_Rect player, SDL_Rect otherHumanoid);  // Make sure player isnt running into something
    
    SDL_Rect attackInFacingDirection(humanoid &attacker);                           // Retrieve hitbox for melee area of effect (AOE)
    
    void levelMainLoop(humanoid &human);                                            // Level main loop
    
    
};

#endif /* GameLevelOne_hpp */
