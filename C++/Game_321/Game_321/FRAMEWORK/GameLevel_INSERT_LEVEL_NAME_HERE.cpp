//
//  GameLevelTutorial.cpp
//  Game_321
//
//  Created by Josh Bosley on 10/12/16.
//  Copyright © 2016 Josh Bosley. All rights reserved.
//

#include "GameLevelTutorial.hpp"

GameLevelTutorial::GameLevelTutorial()
{
    // Load level images and text
    
    levelMainLoop();
}

void GameLevelTutorial::levelMainLoop()
{
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
                    if(_sdla._event.key.keysym.sym == SDLK_w)            // U
                    {
                    }
                    else if(_sdla._event.key.keysym.sym == SDLK_s)       // D
                    {
                    }
                    else if(_sdla._event.key.keysym.sym == SDLK_a)       // L
                    {
                    }
                    else if(_sdla._event.key.keysym.sym == SDLK_d)       // R
                    {
                    }
                    else if(_sdla._event.key.keysym.sym == SDLK_i)
                    {
                    }
                    else if(_sdla._event.key.keysym.sym == SDLK_SPACE)
                    {
                    }
                    else if(_sdla._event.key.keysym.sym == SDLK_ESCAPE)
                    {

                    }
                    else
                    {
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
