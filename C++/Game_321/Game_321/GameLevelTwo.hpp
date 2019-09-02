//
//  GameLevelTwo.hpp
//  Game_321
//
//  Created by Josh Bosley on 10/18/16.
//  Copyright © 2016 Josh Bosley. All rights reserved.
//

#ifndef GameLevelTwo_hpp
#define GameLevelTwo_hpp


#include <stdio.h>
#include <iostream>
#include "common.h"
#include "biped.hpp"
#include "SDLAssist.hpp"

class GameLevelTwo
{
    public :
    GameLevelTwo(humanoid &human);                                                  // Init, call main loop, clean
    
private:
    SDLAssist _sdla;                                                                // SDL Assist class
    
    biped bipedController;                                                          // Manipulate bipeds
    
    SDL_Color textColor;                                                            // text color
    
    bool playerCollided;
    bool keyShowing;
    bool allowFinalComputerUse;
    bool allowSecondRoomCollisions;
    
    enum currentState {fadeIn, explore};
    
    bool checkForNosTalk;                                                           // Handle speaking with nostradoomas
    
    currentState levelState;                                                        // Level state controller
    
    humanoid alienScientist1, alienScientist2, alienGuard1, alienGuard2, nostradoomas;            // Aliens on this level
    
    char itemThatCollidedWithPlayer;
    
    bool doHandleSpriteMovement;                                                    // Toggle ability to move
    
    bool checkForItemObjectColliding(SDL_Rect item);                                // Test all level items
    
    bool checkForPlayerHumanoidColliding(SDL_Rect player, SDL_Rect otherHumanoid);  // Make sure player isnt running into something
    
    void checkForChestAction(humanoid &human);
    
    void checkForWallAction(humanoid &human);
    
    bool checkForAttackRange(humanoid &attacker, humanoid &target);
    
    SDL_Rect attackInFacingDirection(humanoid &attacker);                           // Retrieve hitbox for melee area of effect (AOE)
    
    void updateHeartDisplay(humanoid &human);                                       // Update the heart display
    
    void performBounceBack(humanoid &human);
    
    void levelMainLoop(humanoid &human);                                            // Level main loop
    
    
};
#endif /* GameLevelTwo_hpp */
