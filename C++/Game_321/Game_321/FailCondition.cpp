//
//  FailCondition.cpp
//  Game_321
//
//  Created by Josh Bosley on 11/23/16.
//  Copyright © 2016 Josh Bosley. All rights reserved.
//

#include "FailCondition.hpp"

FailCondition::FailCondition()
{
    _sdla.setupSDL(600, 600, "[WINNER]");
    
    // Setup font color, and display instriction for selection
    
    SDL_Color w = {255,255,255};
    _sdla.createTextItem("instruct", "You've been defeated by the loomans!", "Arial.ttf", w, 100, GAME_SCREEN_HEIGHT/2 - 50, 50, 400);
    _sdla.createTextItem("instruct1", "You have doomed the hooman dimension!", "Arial.ttf", w, 100, GAME_SCREEN_HEIGHT/2 + 55, 50, 400);
    _sdla.createTextItem("instruct1", "Go sit in the corner", "Arial.ttf", w, 100, GAME_SCREEN_HEIGHT/2 + 110, 50, 400);
    mainLoop();
}


void FailCondition::mainLoop()
{
    // Render everything except 'color' for bg use
    _sdla.updateScreen(true, true, false);
    
    while(_sdla._isRunning)
    {
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
                    if(_sdla._event.key.keysym.sym == SDLK_ESCAPE)
                    {
                        exit(0);
                    }
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
