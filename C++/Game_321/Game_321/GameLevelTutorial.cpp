//
//  GameLevelTutorial.cpp
//  Game_321
//
//  Created by Josh Bosley on 10/12/16.
//  Copyright © 2016 Josh Bosley. All rights reserved.
//

#include "GameLevelTutorial.hpp"

GameLevelTutorial::GameLevelTutorial(humanoid  &human)
{
    // Load level images and text
    _sdla.setupSDL(GAME_SCREEN_HEIGHT, GAME_SCREEN_WIDTH, "Joshs Game - Tutorial");
    
    // Load tut background
    _sdla.createScreenItem("bg", "backgrounds/tut.png",
                           0, 0, GAME_SCREEN_HEIGHT, GAME_SCREEN_WIDTH, 0, 0, GAME_SCREEN_HEIGHT, GAME_SCREEN_WIDTH);

    // Information bar
    _sdla.createScreenItem("bar", "overlays/bar.png", 0, 0, 600, 800, 0, 0, GAME_SCREEN_HEIGHT, GAME_SCREEN_WIDTH);
    
    // Init
    doHandleSpriteMovement = true;
    human.spriteRow = 10*64;
    human.spriteColumn += 64;
    levelState = learnMove;
    textColor = {255,255,255};
    
    // Create player sprite
    _sdla.createScreenItem("player", human.image, human.spriteRow, human.spriteColumn, 64, 64, human.x, human.y, 64, 64);
    
    // Create inventory screenn
    _sdla.createScreenItem("inventory", "overlays/inv.png", 0, 0, 600, 800, 0, 0, 600, 800);
    
    // Toggle it off
    _sdla.editScreenItemSetVisibility("inventory", false);
    _sdla.createTextItem("info", "Press W,S,A,D for U,D,L,R movement.", "Arial.ttf",
                         textColor, 100, GAME_SCREEN_HEIGHT-50, 50, 600);
    
    // Run the level
    levelMainLoop(human);
    
    // Once the level is done, free everything that was used before moving on.
    _sdla.killSdlAssist();
}

void GameLevelTutorial::levelMainLoop(humanoid  &human)
{
    unsigned moves = 0;
    bool showingInv = false;
    
    while(_sdla._isRunning)
    {
        // Update sprite image
        _sdla.editScreenItemSourceXY("player", human.spriteColumn, human.spriteRow);
        
        // Update sprite location
        _sdla.editScreenItemDestinationXY("player", (int)human.x, (int)human.y);
        
        // Render everything except 'color' for bg use
        _sdla.updateScreen(true, true, false);
        
        // Test for toggling FROM learn move
        if(levelState == learnMove && moves >= 10)
        {
            if(human.x != GAME_SCREEN_WIDTH/2 && human.y != GAME_SCREEN_HEIGHT/2)
            {
                levelState = learnHealth;
                _sdla.createScreenItem("heart", "sprites/heart.png", 0, 0, 15, 16,
                                       (GAME_SCREEN_WIDTH/2)-10, GAME_SCREEN_HEIGHT/2, 20, 20);
                _sdla.editTextItemMessage("info", "Pick up the heart, by walking over it");
            }
        }
        
        // Test for togglinf FROM learn health
        if(levelState == learnHealth && _sdla.checkForCircleCircleCollision("player", "heart"))
        {
            _sdla.freeScreenItem("heart");
            _sdla.editTextItemMessage("info", "Press 'i' to open inventory");
            levelState = learnInventory;
            
            SDL_Rect s = _sdla.getScreenItemDestinationRectangleData("player");
            _sdla.createScreenItem("ship", "sprites/spaceship.png", 0, 0, 1024, 1024, GAME_SCREEN_WIDTH + 200, s.y, 200, 200);
        }
        
        if(levelState == transition)
        {
            // Get current ship and player info
            SDL_Rect s = _sdla.getScreenItemDestinationRectangleData("ship");
            SDL_Rect t = _sdla.getScreenItemDestinationRectangleData("player");
            
            // Disable charachter movement
            doHandleSpriteMovement = false;
            
            // Disable ability to show inventory
            showingInv = false;
            
            if(s.x != t.x-50)
            {
                _sdla.editScreenItemDestinationXY("ship", s.x-1, t.y-50);
            }
            else
            {
                // Show all the text!
                _sdla.createTextItem("story1", "Hooman! You were the only one who could have stopped our invasion.",
                                     "Arial.ttf", textColor, 100, 20, 25, 600);
                _sdla.createTextItem("story2", "We are the loomans, invading from an alternate dimension!",
                                     "Arial.ttf", textColor, 100, 50, 25, 600);
                _sdla.createTextItem("story3", "We are going to imprison you so you can not foil our plans!",
                                     "Arial.ttf", textColor, 100, 80, 25, 600);
                _sdla.createTextItem("story4", "Prepare to experience unfathomable torment",
                                     "Arial.ttf", textColor, 100, 110, 25, 600);
                _sdla.editTextItemMessage("info", "Press the space bar to continue");
            }
        }
        
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
                        if(doHandleSpriteMovement)
                            bipedController.walkSprite('u', human);
                        
                        if(levelState == learnMove)
                            moves++;
                    }
                    else if(_sdla._event.key.keysym.sym == SDLK_s)       // D
                    {
                        if(doHandleSpriteMovement)
                            bipedController.walkSprite('d', human);
                        
                        if(levelState == learnMove)
                            moves++;
                    }
                    else if(_sdla._event.key.keysym.sym == SDLK_a)       // L
                    {
                        if(doHandleSpriteMovement)
                            bipedController.walkSprite('l', human);
                        
                        if(levelState == learnMove)
                            moves++;
                    }
                    else if(_sdla._event.key.keysym.sym == SDLK_d)       // R
                    {
                        if(doHandleSpriteMovement)
                            bipedController.walkSprite('r', human);
                        
                        if(levelState == learnMove)
                            moves++;
                    }
                    else if(_sdla._event.key.keysym.sym == SDLK_i)
                    {
                        // Toggle inventory
                        if(showingInv == false)
                        {
                            // Only if its time (b/c its a tutorial)
                            if(levelState == learnInventory)
                            {
                                _sdla.editScreenItemSetVisibility("inventory", true);
                                
                                _sdla.createTextItem("iitem", "Your inventory will appear here.", "Arial.ttf",
                                                     textColor, GAME_SCREEN_WIDTH/2 - (GAME_SCREEN_WIDTH/4), GAME_SCREEN_HEIGHT/2, 50, 400);
                                showingInv = true;
                            }
                        }
                        else
                        {
                            showingInv = false;
                            _sdla.editScreenItemSetVisibility("inventory", false);
                            _sdla.freeTextItem("iitem");
                            levelState = transition;
                        }
                    }
                    else if(_sdla._event.key.keysym.sym == SDLK_SPACE)
                    {
                        // Make sure that the ship gets over the player before allowing space press for loading level 1
                        
                        SDL_Rect s = _sdla.getScreenItemDestinationRectangleData("ship");
                        SDL_Rect t = _sdla.getScreenItemDestinationRectangleData("player");
                        
                        if(levelState == transition && s.x == t.x-50)
                        {
                            // End of this level, free everything
                            _sdla._isRunning = false;
                            _sdla.freeTextItem("info");
                            _sdla.freeTextItem("story1");
                            _sdla.freeTextItem("story2");
                            _sdla.freeTextItem("story3");
                            _sdla.freeTextItem("story4");
                            _sdla.freeScreenItem("player");
                            _sdla.freeScreenItem("inventory");
                            _sdla.freeScreenItem("ship");
                            _sdla.freeScreenItem("bar");
                            _sdla.freeScreenItem("bg");
                        }
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
