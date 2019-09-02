//
//  main.cpp
//  Game_321
//
//  Created by Josh Bosley on 10/12/16.
//  Copyright © 2016 Josh Bosley. All rights reserved.
//
//  Main contains char select, and starts off the game!

#include <iostream>

#include "SDLAssist.hpp"            // An SDL helper class that I made. Its pretty sweet. 

#include "common.h"                 // Common structures
#include "GameLevelTutorial.hpp"    // Tutorial class
#include "GameLevelOne.hpp"
#include "GameLevelTwo.hpp"
#include "GameLevelThree.hpp"
#include "WinScreen.hpp"
#include "FailCondition.hpp"

// Player selection prior to game
void selectPlayer(humanoid &human)
{
    // Setup human stats
    human.health = 6.0;
    human.x = 0;
    human.y = 0;
    human.image = "sprites/male_basic.png";
    human.spriteColumn = 64*3;
    human.spriteRow = 0;
    human.armor = 'N';
    human.weapon= 'N';
    
    // Make an instance of my awesome assist class.
    SDLAssist _sdla;
    
    _sdla.setupSDL(600, 600, "[LOOMANS] Player Selection:");
    
    // Setup font color, and display instriction for selection
    SDL_Color w = {255,255,255};
    _sdla.createTextItem("instruct", "Alternate selection using A, and D", "Arial.ttf", w, 100, 20, 50, 400);
    _sdla.createTextItem("s", "PRESS SPACE TO SELECT", "Arial.ttf", w, 100, 500, 50, 400);
    
    // Load male and female to screen to demo selection
    _sdla.createScreenItem("male", "sprites/male_armor.png"  ,
                          human.spriteRow, human.spriteColumn, 64, 64, 150, 200, 64, 64);
    _sdla.createScreenItem("female", "sprites/female_armor.png",
                          human.spriteRow, human.spriteColumn, 64, 64, 400, 200, 64, 64);
    
    // Show make and female text to player. Init human gender
    _sdla.createTextItem("m", "MALE", "Arial.ttf", w, 100, 370, 50, 100);
    _sdla.createTextItem("f", "FEMALE", "Arial.ttf", w, 400, 370, 50, 100);
    _sdla.editTextItemVisibility("f", false);
    human.gender = true;
    
    while(_sdla._isRunning)
    {
        // Render everything except 'color' for bg use
        _sdla.updateScreen(true, true, false);
        
        while(SDL_PollEvent(&_sdla._event))
        {
            switch(_sdla._event.type)
            {
                case SDL_MOUSEBUTTONDOWN:
                {
                    break;
                }
                    
                case SDL_KEYDOWN:
                {
                    if(_sdla._event.key.keysym.sym == SDLK_a)       // L            Toggle gender to male
                    {
                        human.gender = true;
                        _sdla.editTextItemVisibility("m", true);    // Cool way to demo SDLAssist
                        _sdla.editTextItemVisibility("f", false);
                        human.image = "sprites/male_basic.png";
                    }
                    else if(_sdla._event.key.keysym.sym == SDLK_d)       // R       Toggle gender to female
                    {
                        human.gender = false;
                        _sdla.editTextItemVisibility("m", false);
                        _sdla.editTextItemVisibility("f", true);
                        human.image = "sprites/female_basic.png";
                    }
                    else if(_sdla._event.key.keysym.sym == SDLK_SPACE)
                    {                                                   // Cooler way to demo SDLAssist
                        _sdla.freeTextItem("instruct");                 // Free all text items
                        _sdla.freeTextItem("s");                        // Selection text
                        _sdla.freeTextItem("m");                        // Male Text
                        _sdla.freeTextItem("f");                        // Female Text
                        _sdla.freeScreenItem("male");                   // Male image
                        _sdla.freeScreenItem("female");                 // Female image
                        _sdla._isRunning = false;                       // Done with selection. Human gender now set
                    }                                                   // Really, check out SDLAssist
                    break;
                }
                case SDL_QUIT:
                {
                    exit(0);
                    break;
                }
            }
        }
    }
}

void showLoseScreen(humanoid h)
{
  //  std::cout << "YOU LOST M8!!! \n";
    
    FailCondition fail;
    
    exit(0);
}

void showWinScreen(humanoid h)
{
    //std::cout << "YOU WON M8!!! \n";
    
    WinCondition win;
    
    exit(0);
}

int main()
{
    // Create a player
    humanoid humanPlayer;
    // Set their status to indicate they have not yet won
    humanPlayer.win = false;
    // Set their status to indicate nothin can be determined
    humanPlayer.lose = false;
    
// --- Send player to selection screen
    std::cout << "Presenting selection screen. " << std::endl;
    selectPlayer(humanPlayer);
    
    
    
    
// --- Send player to the tutorial
    std::cout << "Initiate tutorial " << std::endl;
    GameLevelTutorial glt(humanPlayer);
    
// --- Send player to level 1
    std::cout << "Initiate level one " << std::endl;
    GameLevelOne glone(humanPlayer);
    
    if(!humanPlayer.win && humanPlayer.lose)
        showLoseScreen(humanPlayer);
    else if (humanPlayer.win && !humanPlayer.lose)
        showWinScreen(humanPlayer);
    
// --- Send player to level 2
    std::cout << "Initiate level two " << std::endl;
    GameLevelTwo gltwo(humanPlayer);
    
    if(!humanPlayer.win && humanPlayer.lose)
        showLoseScreen(humanPlayer);
    else if (humanPlayer.win && !humanPlayer.lose)
        showWinScreen(humanPlayer);
    
// --- Send player to level 3
    std::cout << "Initiate level three " << std::endl;
    GameLevelThree glthree(humanPlayer);
    
    if(!humanPlayer.win && humanPlayer.lose)
        showLoseScreen(humanPlayer);
    else if (humanPlayer.win && !humanPlayer.lose)
        showWinScreen(humanPlayer);
    
    
    
    
    return 0;
}
