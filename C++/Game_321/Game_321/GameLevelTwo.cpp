//
//  GameLevelTwo.cpp
//  Game_321
//
//  Created by Josh Bosley on 10/18/16.
//  Copyright © 2016 Josh Bosley. All rights reserved.
//

#include "GameLevelTwo.hpp"

GameLevelTwo::GameLevelTwo(humanoid &human)
{
    // Init
    human.spriteRow = 11 * 64;
    human.spriteColumn = 0;
    levelState = fadeIn;
    textColor = {255,255,255};
    human.x = 20;
    human.y = GAME_SCREEN_HEIGHT/2 - 64;
    human.health = 6; // 2 per heart
    doHandleSpriteMovement = false;
    itemThatCollidedWithPlayer = ' ';
    keyShowing = false;
    checkForNosTalk = false;
    allowSecondRoomCollisions = false;
    
    // Load level images and text
    _sdla.setupSDL(GAME_SCREEN_HEIGHT, GAME_SCREEN_WIDTH, "Loomans!!!! - Level 2");
    
    // Load tut background
    _sdla.createScreenItem("bg", "backgrounds/level2bg.png",
                           0, 0, GAME_SCREEN_HEIGHT, GAME_SCREEN_WIDTH, 0, 0, GAME_SCREEN_HEIGHT, GAME_SCREEN_WIDTH);
    
    // Information bar
    _sdla.createScreenItem("bar", "overlays/bar.png", 0, 0, 600, 800, 0, 0, GAME_SCREEN_HEIGHT, GAME_SCREEN_WIDTH);
    
    // Update hearts
    updateHeartDisplay(human);
    
    // Create Intrument items for level 2 screen 2
    _sdla.createScreenItem("inst", "sprites/inst.png", 0, 0, 100, 200, GAME_SCREEN_WIDTH-150, 10, 50, 100);
    _sdla.editScreenItemSetVisibility("inst", false);
    _sdla.createScreenItem("inst2", "sprites/inst2.png", 0, 0, 100, 50, GAME_SCREEN_WIDTH-400, GAME_SCREEN_HEIGHT-200, 100, 50);
    _sdla.editScreenItemSetVisibility("inst2", false);
    
    // One more machine
    _sdla.createScreenItem("machine", "sprites/machine.png", 0, 0, 100, 200, GAME_SCREEN_WIDTH-250, GAME_SCREEN_HEIGHT/2 + 110, 50, 100);
    _sdla.editScreenItemSetVisibility("machine", false);
    
    // Create player sprite
    _sdla.createScreenItem("player", human.image, human.spriteRow, human.spriteColumn, 64, 64, human.x, human.y, 64, 64);
    
    // Create chest 'items'
    _sdla.createScreenItem("chest1", "sprites/blank.png", 0, 0, 55, 60, 112, 45, 55, 64);
    _sdla.createScreenItem("chest2", "sprites/blank.png", 0, 0, 55, 60, 200, 45, 55, 64);
    _sdla.createScreenItem("chest3", "sprites/blank.png", 0, 0, 55, 60, 292, 45, 55, 64);
    _sdla.createScreenItem("chest4", "sprites/blank.png", 0, 0, 55, 60, 383, 45, 55, 64);
    _sdla.createScreenItem("chest5", "sprites/blank.png", 0, 0, 55, 60, 472, 45, 55, 64);
    _sdla.createScreenItem("chest6", "sprites/blank.png", 0, 0, 55, 60, 560, 45, 55, 64);
    _sdla.createScreenItem("chest7", "sprites/blank.png", 0, 0, 55,  60, 95, 400, 55, 64);
    _sdla.createScreenItem("chest8", "sprites/blank.png", 0, 0, 55,  60, 190, 400, 55, 64);
    _sdla.createScreenItem("chest9", "sprites/blank.png", 0, 0, 55,  60, 294, 400, 55, 64);
    _sdla.createScreenItem("chest10", "sprites/blank.png", 0, 0, 55, 60, 393, 400, 55, 64);
    _sdla.createScreenItem("chest11", "sprites/blank.png", 0, 0, 55, 60, 486, 400, 55, 64);
    _sdla.createScreenItem("chest12", "sprites/blank.png", 0, 0, 55, 60, 580, 400, 55, 64);
    
    // Create key
    _sdla.createScreenItem("key1", "sprites/key.png", 0, 0, 40, 40, 560, 5, 40, 40);
    _sdla.editScreenItemSetVisibility("key1", false);
    
    // Create doorway
    _sdla.createScreenItem("door1", "sprites/blank.png", 0, 0, 55, 60, GAME_SCREEN_WIDTH-30, GAME_SCREEN_HEIGHT/2 - 85, 160, 35);

    _sdla.createTextItem("userInfo", "Find a key", "Arial.ttf", textColor, GAME_SCREEN_WIDTH-400, GAME_SCREEN_HEIGHT-40, 40, 400);
    
    _sdla.createTextItem("dialog", "Thank Good you found me!", "Arial.ttf", textColor, GAME_SCREEN_WIDTH/2-200, 50, 40, 400);
    _sdla.editTextItemVisibility("dialog", false);
    
    
    
    // Run the main loop
    levelMainLoop(human);
    
    // Clean up everything, and stop display before moving on
    _sdla.killSdlAssist();
}

// Circle-point collision detection
bool GameLevelTwo::checkForPlayerHumanoidColliding(SDL_Rect p, SDL_Rect o)
{
    int radius = p.w/2;                                             // Get radius of the circle
    int radiusSquared = pow(radius, 2);                             // Square it
    int centerX = p.x + (p.w/2);                                    // Get center x,y of the circle
    int centerY = p.y + (p.h/2);
    int distanceSquared = pow(( (o.y + o.h/2) - centerY ), 2);      // Square the distance from point
    distanceSquared += pow(((o.x + o.w/2) - centerX ),2);
    if(radiusSquared > distanceSquared)
        return true;
    else
        return false;
}

// Bounce the player away from an object
void GameLevelTwo::performBounceBack(humanoid &h)
{
    if(h.facing == 'u')
        h.y += PLAYER_MOVEMENT_SPEED;
    else if (h.facing == 'd')
        h.y -= PLAYER_MOVEMENT_SPEED;
    else if (h.facing == 'r')
        h.x -= PLAYER_MOVEMENT_SPEED;
    else
        h.x += PLAYER_MOVEMENT_SPEED;
}

// Check for agro range
bool GameLevelTwo::checkForAttackRange(humanoid &a, humanoid &t)
{
    return ((pow((t.y - a.y),2) + pow((t.x - a.x),2)) < 40000);
}

void GameLevelTwo::checkForChestAction(humanoid &h)
{
    // Get data for placement of chests, and then check if they are colliding
    //  with the player, and respond with pushback or key release
    SDL_Rect p  = _sdla.getScreenItemDestinationRectangleData("player");
    SDL_Rect p1  = _sdla.getScreenItemDestinationRectangleData("chest1");
    SDL_Rect p2  = _sdla.getScreenItemDestinationRectangleData("chest2");
    SDL_Rect p3  = _sdla.getScreenItemDestinationRectangleData("chest3");
    SDL_Rect p4  = _sdla.getScreenItemDestinationRectangleData("chest4");
    SDL_Rect p5  = _sdla.getScreenItemDestinationRectangleData("chest5");
    SDL_Rect p6  = _sdla.getScreenItemDestinationRectangleData("chest6");
    SDL_Rect p7  = _sdla.getScreenItemDestinationRectangleData("chest7");
    SDL_Rect p8  = _sdla.getScreenItemDestinationRectangleData("chest8");
    SDL_Rect p9  = _sdla.getScreenItemDestinationRectangleData("chest9");
    SDL_Rect p10 = _sdla.getScreenItemDestinationRectangleData("chest10");
    SDL_Rect p11 = _sdla.getScreenItemDestinationRectangleData("chest11");
    SDL_Rect p12 = _sdla.getScreenItemDestinationRectangleData("chest12");
    
    if(checkForPlayerHumanoidColliding(p, p1))
    {
        playerCollided = true;
        performBounceBack(h);   // Bounce player away
        _sdla.editTextItemMessage("userInfo", "You find nothing of interest.");
        
    }
    if(checkForPlayerHumanoidColliding(p, p2))
    {
        playerCollided = true;
        performBounceBack(h);   // Bounce player away
        _sdla.editTextItemMessage("userInfo", "You find nothing of interest.");
    }
    if(checkForPlayerHumanoidColliding(p, p3))
    {
        playerCollided = true;
        performBounceBack(h);   // Bounce player away
        _sdla.editTextItemMessage("userInfo", "You find nothing of interest.");
    }
    if(checkForPlayerHumanoidColliding(p, p4))
    {
        playerCollided = true;
        performBounceBack(h);   // Bounce player away
        _sdla.editTextItemMessage("userInfo", "You find nothing of interest.");
    }
    if(checkForPlayerHumanoidColliding(p, p5))
    {
        playerCollided = true;
        performBounceBack(h);   // Bounce player away
        _sdla.editTextItemMessage("userInfo", "You find nothing of interest.");
    }
    if(checkForPlayerHumanoidColliding(p, p6))
    {
        playerCollided = true;
        if(h.facing == 'u')
        {
            h.y += PLAYER_MOVEMENT_SPEED;
            if(!keyShowing)
            {
                _sdla.editScreenItemSetVisibility("key1", true);
                keyShowing = true;
            }
        }
        else if (h.facing == 'd')
        {
            h.y -= PLAYER_MOVEMENT_SPEED;
            if(!keyShowing)
            {
                _sdla.editScreenItemDestinationXY("key1", 560, 95);
                _sdla.editScreenItemSetVisibility("key1", true);
                keyShowing = true;

            }
        }
        else if (h.facing == 'r')
        {
            h.x -= PLAYER_MOVEMENT_SPEED;
            if(!keyShowing)
            {
                _sdla.editScreenItemSetVisibility("key1", true);
                keyShowing = true;
            }
        }
        else
        {
            if(!keyShowing)
            {
                _sdla.editScreenItemSetVisibility("key1", true);
                keyShowing = true;
            }
            h.x += PLAYER_MOVEMENT_SPEED;
        }
        _sdla.editTextItemMessage("userInfo", "A key has jumped out of the chest.");
    }
    
    if(checkForPlayerHumanoidColliding(p, p7))
    {
        playerCollided = true;
        performBounceBack(h);   // Bounce player away
        _sdla.editTextItemMessage("userInfo", "You find nothing of interest.");
    }
    if(checkForPlayerHumanoidColliding(p, p8))
    {
        playerCollided = true;
        performBounceBack(h);   // Bounce player away
        _sdla.editTextItemMessage("userInfo", "You find nothing of interest.");
    }
    if(checkForPlayerHumanoidColliding(p, p9))
    {
        playerCollided = true;
        performBounceBack(h);   // Bounce player away
        _sdla.editTextItemMessage("userInfo", "You find nothing of interest.");
    }
    if(checkForPlayerHumanoidColliding(p, p10))
    {
        playerCollided = true;
        performBounceBack(h);   // Bounce player away
        _sdla.editTextItemMessage("userInfo", "You find nothing of interest.");
    }
    if(checkForPlayerHumanoidColliding(p, p11))
    {
        playerCollided = true;
        performBounceBack(h);   // Bounce player away
        _sdla.editTextItemMessage("userInfo", "You find nothing of interest.");
    }
    if(checkForPlayerHumanoidColliding(p, p12))
    {
        playerCollided = true;
        performBounceBack(h);   // Bounce player away
        _sdla.editTextItemMessage("userInfo", "You find nothing of interest.");
    }
}

void GameLevelTwo::checkForWallAction(humanoid &h)
{
    if(allowSecondRoomCollisions)
    {
    // Check for center of player colliding with wall boundary. Respond with push back
    SDL_Rect p = _sdla.getScreenItemDestinationRectangleData("player");
    int px = p.x + (p.w/2);
    int py = p.y + (p.h/2);
    SDL_Rect w1 = _sdla.getScreenItemDestinationRectangleData("wall1");
    SDL_Rect w2 = _sdla.getScreenItemDestinationRectangleData("wall2");
    SDL_Rect w3 = _sdla.getScreenItemDestinationRectangleData("wall3");
    SDL_Rect w4 = _sdla.getScreenItemDestinationRectangleData("wall4");
    SDL_Rect w5 = _sdla.getScreenItemDestinationRectangleData("wall5");
    SDL_Rect w6 = _sdla.getScreenItemDestinationRectangleData("wall6");

    // Also check for collision of items in the room that cant be ran into
    // And clearly, these if statements could be a single line, but I think broken
    // out looks nicer.
    if((px >= w1.x) && (px <= (w1.x + w1.w)))     // Box one width
    {
        if(((py >= w1.y) && (py <= (w1.y + w1.h))))
        {
            performBounceBack(h);   // Bounce player away
        }
    }

    if((px >= w2.x) && (px <= (w2.x + w2.w)))     // Box two width
    {
        if(((py >= w2.y) && (py <= (w2.y + w2.h))))
        {
            performBounceBack(h);   // Bounce player away
        }
    }
    
    if((px >= w3.x) && (px <= (w3.x + w3.w)))     // Box 3 width
    {
        if(((py >= w3.y) && (py <= (w3.y + w3.h))))
        {
            performBounceBack(h);   // Bounce player away
        }
    }
    
    if((px >= w4.x) && (px <= (w4.x + w4.w)))     // Box 4 width
    {
        if(((py >= w4.y) && (py <= (w4.y + w4.h))))
        {
            performBounceBack(h);   // Bounce player away
        }
    }
    
    if((px >= w5.x) && (px <= (w5.x + w5.w)))     // Box 5 width
    {
        if(((py >= w5.y) && (py <= (w5.y + w5.h))))
        {
            performBounceBack(h);   // Bounce player away
        }
    }
    
    if((px >= w6.x) && (px <= (w6.x + w6.w)))     // 6 two width
    {
        if(((py >= w6.y) && (py <= (w6.y + w6.h))))
        {
            performBounceBack(h);   // Bounce player away
        }
    }
    
    // check agains instruments
    SDL_Rect ins = _sdla.getScreenItemDestinationRectangleData("inst");
    SDL_Rect ins2 = _sdla.getScreenItemDestinationRectangleData("inst2");
    SDL_Rect mach = _sdla.getScreenItemDestinationRectangleData("machine");

    if((px >= ins.x) && (px <= (ins.x + ins.w)))
    {
        if(((py >= ins.y) && (py <= (ins.y + ins.h))))
        {
            performBounceBack(h);   // Bounce player away
        }
    }
    if((px >= ins2.x) && (px <= (ins2.x + ins2.w)))
    {
        if(((py >= ins2.y) && (py <= (ins2.y + ins2.h))))
        {
            performBounceBack(h);   // Bounce player away
        }
    }
    if((px >= mach.x) && (px <= (mach.x + mach.w)))
    {
        if(((py >= mach.y) && (py <= (mach.y + mach.h))))
        {
            performBounceBack(h);   // Bounce player away
        }
    }

    // Check against sub-roomdoorways
    SDL_Rect d1 = _sdla.getScreenItemDestinationRectangleData("door2");
    SDL_Rect d2 = _sdla.getScreenItemDestinationRectangleData("door3");
    SDL_Rect d3 = _sdla.getScreenItemDestinationRectangleData("door4");
    SDL_Rect d4 = _sdla.getScreenItemDestinationRectangleData("door5");
    
    if((px >= d1.x) && (px <= (d1.x + d1.w)))
    {
        if(((py >= d1.y) && (py <= (d1.y + d1.h))))
        {
            performBounceBack(h);   // Bounce player away
        }
    }
    if((px >= d2.x) && (px <= (d2.x + d2.w)))
    {
        if(((py >= d2.y) && (py <= (d2.y + d2.h))))
        {
            performBounceBack(h);   // Bounce player away
        }
    }
    if((px >= d3.x) && (px <= (d3.x + d3.w)))
    {
        if(((py >= d3.y) && (py <= (d3.y + d3.h))))
        {
            performBounceBack(h);   // Bounce player away
        }
    }
    if((px >= d4.x) && (px <= (d4.x + d4.w)))
    {
        if(((py >= d4.y) && (py <= (d4.y + d4.h))))
        {
            performBounceBack(h);   // Bounce player away
        }
    }
    
    // Non-damage giving NPC nostradoomas
    SDL_Rect nos = _sdla.getScreenItemDestinationRectangleData("nostradoomas");
    if((px >= nos.x) && (px <= (nos.x + nos.w)))
    {
        if(((py >= nos.y) && (py <= (nos.y + nos.h))))
        {
            performBounceBack(h);   // Bounce player away
        }
    }
}
}

bool GameLevelTwo::checkForItemObjectColliding(SDL_Rect item)
{
    if(allowSecondRoomCollisions)
    {
        // Used to check for hits against enemy npc, and players collisions with them
        SDL_Rect alienOne = _sdla.getScreenItemDestinationRectangleData("alienScientist1");
        SDL_Rect alienTwo = _sdla.getScreenItemDestinationRectangleData("alienScientist2");
        SDL_Rect ag1 = _sdla.getScreenItemDestinationRectangleData("alienGuard1");

        if( alienScientist1.health > 0 && checkForPlayerHumanoidColliding(item, alienOne) )
        {
            itemThatCollidedWithPlayer = '1';
            return true;
        }
        if( alienScientist2.health > 0 && checkForPlayerHumanoidColliding(item, alienTwo) )
        {
            itemThatCollidedWithPlayer = '2';
            return true;
        }
        if( alienGuard1.health > 0 && checkForPlayerHumanoidColliding(item, ag1) )
        {
            itemThatCollidedWithPlayer = '3';
            return true;
        }
        
        // Check for hitting machines
        SDL_Rect inst = _sdla.getScreenItemDestinationRectangleData("inst");
        SDL_Rect inst2 = _sdla.getScreenItemDestinationRectangleData("inst2");
        SDL_Rect mach = _sdla.getScreenItemDestinationRectangleData("machine");
        
        if( checkForPlayerHumanoidColliding(item, inst) )
        {
            itemThatCollidedWithPlayer = '4';
            return true;
        }
        if( checkForPlayerHumanoidColliding(item, inst2) )
        {
            itemThatCollidedWithPlayer = '5';
            return true;
        }
        if( checkForPlayerHumanoidColliding(item, mach) )
        {
            itemThatCollidedWithPlayer = '6';
            return true;
        }
        
        // Check for NOS collide
       
        if( checkForNosTalk )                                         // This is a work-around to ensure NOS was actually set
        {
            SDL_Rect nos = _sdla.getScreenItemDestinationRectangleData("nostradoomas");
            if( checkForPlayerHumanoidColliding(item, nos) )
            {
                itemThatCollidedWithPlayer = '7';
                return true;
            }
        }
        
        // If allowed to use the computer to open the door, then check for computer use
        if(allowFinalComputerUse)
        {
            SDL_Rect n = _sdla.getScreenItemSourceRectangleData("inst");
            if(checkForPlayerHumanoidColliding(item, n))
            {
                itemThatCollidedWithPlayer = '8';
                return true;
            }
        }
    }
    // Check other
    return false;
}

// Only for 64x64 humanoids
SDL_Rect GameLevelTwo::attackInFacingDirection(humanoid &attacker)
{
    SDL_Rect areaOfEffect;
    areaOfEffect.h = 64;
    areaOfEffect.w = 64;
    
    // For debugging
 //    _sdla.freeScreenItem("hitbox");
    
    // Create 32x32 hit box in direction of attack centered on side of attack
    switch(attacker.facing)
    {
        case 'u':
            areaOfEffect.x = attacker.x;
            areaOfEffect.y = attacker.y - 40;
            break;
        case 'd':
            areaOfEffect.x = attacker.x;
            areaOfEffect.y = attacker.y + 74;
            break;
        case 'l':
            areaOfEffect.x = attacker.x - 64;
            areaOfEffect.y = attacker.y;
            break;
        case 'r':
            areaOfEffect.x = attacker.x + 64;
            areaOfEffect.y = attacker.y ;
            break;
    }
    // For debugging
 //      _sdla.createScreenItem("hitbox", "sprites/hitbox.png", 0, 0, 64, 64, areaOfEffect.x, areaOfEffect.y, 64, 64);
    return areaOfEffect;
}

// Draw hearts
void GameLevelTwo::updateHeartDisplay(humanoid &h)
{
    _sdla.freeScreenItem("heart1");         // Free hearts
    _sdla.freeScreenItem("heart2");
    _sdla.freeScreenItem("heart3");
    _sdla.freeScreenItem("heart1h");        // Free half hearts
    _sdla.freeScreenItem("heart2h");
    _sdla.freeScreenItem("heart3h");
    
    // Draw hearts as-per health
    switch((int)h.health)
    {
        case 1:
            _sdla.createScreenItem("heart1h", "sprites/heart_half.png", 0, 0, 15, 16, 50, GAME_SCREEN_HEIGHT-30, 20, 20);
            break;
        case 2:
            _sdla.createScreenItem("heart1", "sprites/heart.png", 0, 0, 15, 16, 50, GAME_SCREEN_HEIGHT-30, 20, 20);
            break;
        case 3:
            _sdla.createScreenItem("heart1", "sprites/heart.png", 0, 0, 15, 16, 50, GAME_SCREEN_HEIGHT-30, 20, 20);
            _sdla.createScreenItem("heart2h", "sprites/heart_half.png", 0, 0, 15, 16, 90, GAME_SCREEN_HEIGHT-30, 20, 20);
            break;
        case 4:
            _sdla.createScreenItem("heart1", "sprites/heart.png", 0, 0, 15, 16, 50, GAME_SCREEN_HEIGHT-30, 20, 20);
            _sdla.createScreenItem("heart2", "sprites/heart.png", 0, 0, 15, 16, 90, GAME_SCREEN_HEIGHT-30, 20, 20);
            break;
        case 5:
            _sdla.createScreenItem("heart1", "sprites/heart.png", 0, 0, 15, 16, 50, GAME_SCREEN_HEIGHT-30, 20, 20);
            _sdla.createScreenItem("heart2", "sprites/heart.png", 0, 0, 15, 16, 90, GAME_SCREEN_HEIGHT-30, 20, 20);
            _sdla.createScreenItem("heart3h", "sprites/heart_half.png", 0, 0, 15, 16, 130, GAME_SCREEN_HEIGHT-30, 20, 20);
            break;
        case 6:
            _sdla.createScreenItem("heart1", "sprites/heart.png", 0, 0, 15, 16, 50, GAME_SCREEN_HEIGHT-30, 20, 20);
            _sdla.createScreenItem("heart2", "sprites/heart.png", 0, 0, 15, 16, 90, GAME_SCREEN_HEIGHT-30, 20, 20);
            _sdla.createScreenItem("heart3", "sprites/heart.png", 0, 0, 15, 16, 130, GAME_SCREEN_HEIGHT-30, 20, 20);
            break;
        default:
            // If they don't have a health its safe to say they died
            h.win = false;
            h.lose = true;
            break;
    }
}

void GameLevelTwo::levelMainLoop(humanoid &human)
{
    // Story control
    int spaceTap = 0;
    bool allowStoryStep = false;
    playerCollided = false;
    int numberOfCollisions = 0; // For pushing user away when in collision
    int fadeSpeedDelta = 0;
    int fadeSpeedControll = 50;
    bool showingInv = false;
    
    // Player reset controlls
    SDL_Rect tempRect;
    int playerLastX = 0, playerLastY = 0;
    
    // Current room                                                     ----------- SET TO 0 TO START FROM BEGINNING
    unsigned room = 0;
    bool hasKeyForRoomOne = false;
    bool hasKeyForRoomTwo = false;
    
    allowFinalComputerUse = false;
    
    // Main loop
    while(_sdla._isRunning)
    {
        // Check for player deaths
        if(human.health <= 0)
        {
            _sdla._isRunning = false;
            human.win = false; human.lose = true;
        }
        
        // I know
    avoidClippingOnCollision:
        // Update player image and location
        _sdla.editScreenItemSourceXY("player", human.spriteColumn, human.spriteRow);
        _sdla.editScreenItemDestinationXY("player", (int)human.x, (int)human.y);
        
        // Check player sprite is colliding with aliens
        playerCollided = false;
        if(checkForItemObjectColliding(_sdla.getScreenItemDestinationRectangleData("player")))
        {
            // Since collisions happen pretty quickly, take 1/20th of 1 point every 4 hits.
            // This allows for plenty reaction time from the player for a simple collision.
            // Health of the player starts at 6, So 0.1 is a total of 60 hit points on collision
            // Taking damage every 4 hits, lets the player collide 240 times with an enemy (not being attacked)
            if(itemThatCollidedWithPlayer == '1')
            {
                if(numberOfCollisions % 4 == 0)
                    human.health -= 0.1;
            }
            
            // Update the current health
            updateHeartDisplay(human);

            if(numberOfCollisions == 0)
            {
                // Try pushing back one to see if it works, if not, then we'll have to push
                // back as far away as the number of collisions
                human.x = playerLastX;
                human.y = playerLastY;
            }
            else
            {
                // If a collision hang happens, push back the number of collisions away from where
                // first collision took place
                switch(human.facing)
                {
                    case 'u':
                        human.x = playerLastX;
                        human.y = playerLastY + numberOfCollisions;
                        break;
                    case 'd':
                        human.x = playerLastX;
                        human.y = playerLastY - numberOfCollisions;
                        break;
                    case 'l':
                        human.x = playerLastX + numberOfCollisions;
                        human.y = playerLastY;
                        break;
                    case 'r':
                        human.x = playerLastX - numberOfCollisions;
                        human.y = playerLastY;
                        break;
                }
            }
            
            playerCollided = true;
            // I know I shouldn't do this, but its this, or surround everything in an if
            numberOfCollisions++;
            goto avoidClippingOnCollision;
        }
        
        // Check for collision with door 1
        if(room == 0 &&
           checkForPlayerHumanoidColliding(_sdla.getScreenItemDestinationRectangleData("player"),
                                           _sdla.getScreenItemDestinationRectangleData("door1")))
        {
       //     std::cout << "Hey, remember that we have to comment out the if(true) < Line 645\n";
            // CURRENT BYBASS
            if(hasKeyForRoomOne)
      //      if(true)
            {
                
                // Setup room 2
                _sdla.editScreenItemSourceXY("bg", GAME_SCREEN_WIDTH, 0);
                _sdla.editTextItemMessage("userInfo", "The door opens..");
                SDL_Rect p = _sdla.getScreenItemDestinationRectangleData("player");
                human.x = 25; human.y = p.y;
                keyShowing = false;
                room = 1;
                
                // Create sub-room two's NPC1
                alienScientist1.x = 50; alienScientist1.y = 50;
                alienScientist1.health = 6;
                alienScientist1.droppedItem = false;
                alienScientist1.gender = true;
                alienScientist1.speedDelta = 50;
                alienScientist1.moveSwitch = 'r';
                alienScientist1.spriteRow = 11*64;
                alienScientist1.spriteColumn = 0;
                _sdla.createScreenItem("alienScientist1", "sprites/alien_scientist.png",
                                       0, 11 * 64, 64, 64, alienScientist1.x, alienScientist1.y, 64, 64);
                
                // Create sub-room two's NPC2
                alienScientist2.x = 50; alienScientist2.y = GAME_SCREEN_HEIGHT-150;
                alienScientist2.health = 6;
                alienScientist2.droppedItem = false;
                alienScientist2.gender = true;
                alienScientist2.speedDelta = 50;
                alienScientist2.moveSwitch = 'r';
                alienScientist2.spriteRow = 11*64;
                alienScientist2.spriteColumn = 0;
                _sdla.createScreenItem("alienScientist2", "sprites/alien_scientist.png",
                                       0, 11 * 64, 64, 64, alienScientist2.x, alienScientist2.y, 64, 64);
                
                // Create sub-room two's NPC3
                nostradoomas.x = GAME_SCREEN_WIDTH-150; nostradoomas.y = 75;
                nostradoomas.health = 6;
                nostradoomas.droppedItem = false;
                nostradoomas.gender = true;
                nostradoomas.speedDelta = 200;
                nostradoomas.moveSwitch = 'r';
                nostradoomas.spriteRow = 11*64;
                nostradoomas.spriteColumn = 0;
                _sdla.createScreenItem("nostradoomas", "sprites/nostradoomas.png",
                                       0, 11 * 64, 64, 64, nostradoomas.x, nostradoomas.y, 64, 64);
                
                // Create sub-room two's NPC4
                alienGuard1.x = GAME_SCREEN_WIDTH-150; alienGuard1.y = GAME_SCREEN_HEIGHT/2;
                alienGuard1.health = 6;
                alienGuard1.droppedItem = false;
                alienGuard1.gender = false;
                alienGuard1.speedDelta = 50;
                alienGuard1.hitDelta = 0;
                alienGuard1.moveSwitch = 'l';
                alienGuard1.spriteRow = 11*64;
                alienGuard1.spriteColumn = 0;
                alienGuard1.inCombat = false;
                _sdla.createScreenItem("alienGuard1", "sprites/alien_female.png",
                                       0, 11 * 64, 64, 64, alienGuard1.x, alienGuard1.y, 64, 64);
                
                // Create boundaries for sub-room two
                _sdla.createScreenItem("wall1", "sprites/blank.png", 0, 0, 55, 60, 20, GAME_SCREEN_HEIGHT/2 - 145, 60, 245);
                _sdla.createScreenItem("wall2", "sprites/blank.png", 0, 0, 55, 60, 20, GAME_SCREEN_HEIGHT/2 + 50, 60, 245);
                _sdla.createScreenItem("wall3", "sprites/blank.png", 0, 0, 55, 60, 335, GAME_SCREEN_HEIGHT/2 - 150, 60, 343);
                _sdla.createScreenItem("wall4", "sprites/blank.png", 0, 0, 55, 60, 334, GAME_SCREEN_HEIGHT/2 + 50, 60, 338);
                _sdla.createScreenItem("wall5", "sprites/blank.png", 0, 0, 55, 60, GAME_SCREEN_WIDTH/2 + 34, 0 , 190, 60);
                _sdla.createScreenItem("wall6", "sprites/blank.png", 0, 0, 55, 60, GAME_SCREEN_WIDTH/2 + 30, 400 , 190, 60);
                
                // Draw room doors
                _sdla.createScreenItem("door2", "sprites/doorBlock.png", 0, 0, 200, 60, 265, GAME_SCREEN_HEIGHT/2 - 135, 60, 80);
                _sdla.createScreenItem("door3", "sprites/doorBlock.png", 0, 0, 200, 60, 265, GAME_SCREEN_HEIGHT/2 + 60, 60, 80);
                _sdla.createScreenItem("door4", "sprites/doorBlock.png", 0, 0, 200, 60, 670, GAME_SCREEN_HEIGHT/2 - 135, 60, 120);
                _sdla.createScreenItem("door5", "sprites/doorBlock.png", 0, 0, 200, 60, 670, GAME_SCREEN_HEIGHT/2 + 65, 60, 120);
                
                // Show instruments
                _sdla.editScreenItemSetVisibility("inst", true);
                _sdla.editScreenItemSetVisibility("inst2", true);
                _sdla.editScreenItemSetVisibility("machine", true);
                
                allowSecondRoomCollisions = true;
            }
            else
            {
                // only 'dead' temporarily
                _sdla.editTextItemMessage("userInfo", "You need a key to open this. ");
            }
        }
        else if (room == 1 &&
                 checkForPlayerHumanoidColliding(_sdla.getScreenItemDestinationRectangleData("player"),
                                                 _sdla.getScreenItemDestinationRectangleData("door1")))
        {
            // If the player has the key
            if(hasKeyForRoomTwo)
            {
                // Open the door and indicate its opening on the screen
                _sdla.editTextItemMessage("userInfo", "The door opens..");
                SDL_Rect p = _sdla.getScreenItemDestinationRectangleData("player");
                human.x = 25; human.y = p.y;
                
                _sdla.editScreenItemSourceXY("bg", GAME_SCREEN_WIDTH*2, 0);
                room = 2;
                keyShowing = false;
            }
            else
            {
                if(allowFinalComputerUse)
                {
                    // Kill the game, to send to level three
                    _sdla._isRunning = false;
                }
                else
                    _sdla.editTextItemMessage("userInfo", "This door is opened somewhere else.");
            }
        }
        
        // Check for room 0 boundaries
        if(room == 0)
        {
            // Check for chest collision
            checkForChestAction(human);
            
            // If the hey is on screen, check for collision
            if(keyShowing)
            {
                // See if they grab the key
                SDL_Rect k = _sdla.getScreenItemDestinationRectangleData("key1");
                SDL_Rect p = _sdla.getScreenItemDestinationRectangleData("player");
                if(checkForPlayerHumanoidColliding(p, k))
                {
                    _sdla.editScreenItemSetVisibility("key1", false);
                    _sdla.editTextItemMessage("userInfo", "You picked up a key!");
                    hasKeyForRoomOne = true;
                }
            }
        }
        
        // Check for room 1 boundaries
        if(room == 1)
        {
            // Ensure player doesn't phase through walls
            checkForWallAction(human);
            
            // Update NPCs in room 1
            
            // Update Alien Scientist 1
            if(alienScientist1.speedDelta == 50)
            {
                alienScientist1.speedDelta = 0;
                SDL_Rect as1 = _sdla.getScreenItemDestinationRectangleData("alienScientist1");
                
                // Move left and right
                if(alienScientist1.moveSwitch == 'r')
                    if(as1.x < 250)
                        bipedController.walkSprite('r', alienScientist1);
                    else
                        alienScientist1.moveSwitch = 'l';
                else
                    if(as1.x > 50)
                        bipedController.walkSprite('l', alienScientist1);
                    else
                        alienScientist1.moveSwitch = 'r';
                
                // Update location and source
                _sdla.editScreenItemSourceXY("alienScientist1", alienScientist1.spriteColumn, alienScientist1.spriteRow);
                _sdla.editScreenItemDestinationXY("alienScientist1", (int)alienScientist1.x, (int)alienScientist1.y);
            }
            else
                alienScientist1.speedDelta++;
            
            // Update Alien 2
            if(alienScientist2.speedDelta == 50)
            {
                alienScientist2.speedDelta = 0;
                SDL_Rect as1 = _sdla.getScreenItemDestinationRectangleData("alienScientist2");
                
                // Move left and right
                if(alienScientist2.moveSwitch == 'r')
                    if(as1.x < 350)
                        bipedController.walkSprite('r', alienScientist2);
                    else
                        alienScientist2.moveSwitch = 'l';
                    else
                        if(as1.x > 50)
                            bipedController.walkSprite('l', alienScientist2);
                        else
                            alienScientist2.moveSwitch = 'r';
                
                // Update location and source
                _sdla.editScreenItemSourceXY("alienScientist2", alienScientist2.spriteColumn, alienScientist2.spriteRow);
                _sdla.editScreenItemDestinationXY("alienScientist2", (int)alienScientist2.x, (int)alienScientist2.y);
            }
            else
                alienScientist2.speedDelta++;
            
            // Update Alien Guard 1
            // Check to see if player is in range to attack them
            (checkForAttackRange(alienGuard1, human)) ? alienGuard1.inCombat = true : alienGuard1.inCombat = false;
            
            if(!alienGuard1.inCombat && (!checkForPlayerHumanoidColliding(_sdla.getScreenItemDestinationRectangleData("player")
                                                                          , _sdla.getScreenItemDestinationRectangleData("alienGuard1"))))
            {
                if(alienGuard1.speedDelta == 50)
                {
                    alienGuard1.speedDelta = 0;
                    SDL_Rect as1 = _sdla.getScreenItemDestinationRectangleData("alienGuard1");
                    // Move left and right
                    if(as1.x >= GAME_SCREEN_WIDTH/2 && (alienGuard1.moveSwitch == 'l'))
                        bipedController.walkSprite('l', alienGuard1);
                    else if (as1.x <= GAME_SCREEN_WIDTH/2 && (alienGuard1.moveSwitch == 'l'))
                        alienGuard1.moveSwitch = 'r';
                    else if (alienGuard1.moveSwitch == 'r' && as1.x <= GAME_SCREEN_WIDTH-100)
                        bipedController.walkSprite('r', alienGuard1);
                    else
                        alienGuard1.moveSwitch = 'l';
                    // Update location and source
                    _sdla.editScreenItemSourceXY("alienGuard1", alienGuard1.spriteColumn, alienGuard1.spriteRow);
                    _sdla.editScreenItemDestinationXY("alienGuard1", (int)alienGuard1.x, (int)alienGuard1.y);
                }
                else
                    alienGuard1.speedDelta++;
            }
            else
            {
                // Pursue the player
               // /*
                SDL_Rect p = _sdla.getScreenItemDestinationRectangleData("player");
                SDL_Rect a = _sdla.getScreenItemDestinationRectangleData("alienGuard1");
                
                if(alienGuard1.speedDelta > 50 && alienGuard1.health > 0)
                {
                    alienGuard1.speedDelta = 0;
                    if( (abs(p.x - a.x) >= a.w )  || (abs(p.y - a.y) >= a.h ))
                    {
                        std::cout << "Humans and aliens have collided! [894] !\n";
                        alienGuard1.inCombat = false;
                        if(a.y > p.y)
                        {
                            bipedController.walkSprite('u', alienGuard1);
                        }
                        else
                        {
                            bipedController.walkSprite('d', alienGuard1);
                        }
                        if(a.x > p.x)
                        {
                            bipedController.walkSprite('l', alienGuard1);
                        }
                        else
                        {
                            bipedController.walkSprite('r', alienGuard1);
                        }
                    }
                    else
                    {
                        if(alienGuard1.hitDelta == 5)
                        {
                            alienGuard1.hitDelta = 0;
                            // Check to see if the alien's attack will be hitting the player
                            if(checkForPlayerHumanoidColliding(  attackInFacingDirection(alienGuard1), _sdla.getScreenItemDestinationRectangleData("player") ))
                            {
                                if( human.image == "sprites/male_armor_dagger.png" || human.image == "sprites/female_armor_dagger.png" ||
                                    human.image == "sprites/male_armor.png" || human.image == "sprites/female_armor.png" )
                                {
                                    human.health -= 1;
                                }
                                else{
                                    human.health -=2;
                                }
                                // Update the heart display
                                updateHeartDisplay(human);
                            }
                        }
                        else
                            alienGuard1.hitDelta ++;
                    }
                    _sdla.editScreenItemSourceXY("alienGuard1", alienGuard1.spriteColumn, alienGuard1.spriteRow);
                    _sdla.editScreenItemDestinationXY("alienGuard1", (int)alienGuard1.x, (int)alienGuard1.y);
                }
                else
                    alienGuard1.speedDelta++;
            }
        } // END ROOM ONE CHECK
        
        // SWITCH BY STATE HERE
        
        switch(levelState)
        {
            case fadeIn:
            {
                if(fadeSpeedControll >= 20)
                {
                    fadeSpeedControll = 0;
                    _sdla.setRenderDrawColor(255, 255, 255, 100);
                    fadeSpeedDelta += 50;
                    if(fadeSpeedDelta == 250)
                    {
                        levelState = explore;
                    }
                }
                else
                    fadeSpeedControll++;
                break;
            }
                
            case explore:
                
                doHandleSpriteMovement = true;
                
                // check for enemies attacking hooman player
                
                break;
                
            default:
                break;
        }
        
        // Render !!!!!
        if(fadeIn == levelState)
        {
            _sdla.updateScreen(false, false, true);
            SDL_Delay(5);
        }
        else
            _sdla.updateScreen(true, true, true);
        
        // HANDLE ALL INPUT EVENTS
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
                        if(doHandleSpriteMovement && !playerCollided)
                        {
                            // Store players xy in case they do something dumb (run into an alien)
                            tempRect = _sdla.getScreenItemDestinationRectangleData("player");
                            playerLastX = tempRect.x; playerLastY = tempRect.y;
                            numberOfCollisions = 0;
                            bipedController.walkSprite('u', human);
                        }
                    }
                    else if(_sdla._event.key.keysym.sym == SDLK_s)       // D
                    {
                        if(doHandleSpriteMovement && !playerCollided)
                        {
                            // Store players xy in case they do something dumb (run into an alien)
                            tempRect = _sdla.getScreenItemDestinationRectangleData("player");
                            playerLastX = tempRect.x; playerLastY = tempRect.y;
                            numberOfCollisions = 0;
                            bipedController.walkSprite('d', human);
                        }
                    }
                    else if(_sdla._event.key.keysym.sym == SDLK_a)       // L
                    {
                        if(doHandleSpriteMovement && !playerCollided)
                        {
                            // Store players xy in case they do something dumb (run into an alien)
                            tempRect = _sdla.getScreenItemDestinationRectangleData("player");
                            playerLastX = tempRect.x; playerLastY = tempRect.y;
                            numberOfCollisions = 0;
                            bipedController.walkSprite('l', human);
                        }
                    }
                    else if(_sdla._event.key.keysym.sym == SDLK_d)       // R
                    {
                        if(doHandleSpriteMovement && !playerCollided)
                        {
                            // Store players xy in case they do something dumb (run into an alien)
                            tempRect = _sdla.getScreenItemDestinationRectangleData("player");
                            playerLastX = tempRect.x; playerLastY = tempRect.y;
                            numberOfCollisions = 0;
                            bipedController.walkSprite('r', human);
                        }
                    }
                    else if(_sdla._event.key.keysym.sym == SDLK_i)
                    {
                        // Toggle inventory
                        if(showingInv == false)
                        {
                            // Create inventory screen
                            _sdla.createScreenItem("inventory", "overlays/inv.png", 0, 0, 600, 800, 0, 0, 600, 800);
                            _sdla.editScreenItemSetVisibility("inventory", true);
                            
                            
                            if( human.image == "sprites/male_basic.png" || human.image == "sprites/female_basic.png"  )
                            {
                                _sdla.createTextItem("iitem", "Your inventory will appear here.", "Arial.ttf",
                                                     textColor, GAME_SCREEN_WIDTH/2 - (GAME_SCREEN_WIDTH/4), GAME_SCREEN_HEIGHT/2, 50, 400);
                            }
                            
                            if(human.image == "sprites/male_basic_dagger.png" || human.image == "sprites/male_armor_dagger.png" ||
                               human.image == "sprites/female_basic_dagger.png" || human.image == "sprites/female_armor_dagger.png")
                            {
                                _sdla.createScreenItem("knifeStar1_", "sprites/knife_star.png",
                                                       0, 0, 150, 150, GAME_SCREEN_WIDTH/2 - 250, GAME_SCREEN_HEIGHT/2-100, 150, 150);
                                
                                
                            }
                            
                            if(human.image == "sprites/male_armor_dagger.png" || human.image == "sprites/female_armor_dagger.png" ||
                               human.image == "sprites/male_armor.png" || human.image == "sprites/female_armor.png"
                               
                               )
                            {
                                
                                _sdla.createScreenItem("armor_", "sprites/armor_pickup.png",
                                                       0, 0, 150, 150, GAME_SCREEN_WIDTH/2 + 50, GAME_SCREEN_HEIGHT/2-100, 150, 150);
                            }
                            

                            showingInv = true;
                        }
                        else
                        {
                            showingInv = false;
                            _sdla.freeScreenItem("inventory");
                            _sdla.freeTextItem("iitem");
                            _sdla.freeScreenItem("knifeStar1_");
                            _sdla.freeScreenItem("armor_");
                        }
                    }
                    else if(_sdla._event.key.keysym.sym == SDLK_SPACE)
                    {
                        // Handle combat, or story progression
                        if (allowStoryStep)
                        {
                            // Story progression
                            switch(spaceTap)
                            {
                                case 1:
                                    _sdla.editTextItemMessage("dialog", "I was beginning to think you couldn't make it ...");
                                    break;
                                case 2:
                                    _sdla.editTextItemMessage("dialog", "In order to save your dimension, you'll ...");
                                    break;
                                case 3:
                                    _sdla.editTextItemMessage("dialog", "have to defeat the Kiing GRIZNIAK! ...");
                                    break;
                                case 4:
                                    _sdla.editTextItemMessage("dialog", "As you know, he leads the loomans ...");
                                    break;
                                case 5:
                                    _sdla.editTextItemMessage("dialog", "This it the 8th dimension he has taken ...");
                                    break;
                                case 6:
                                    _sdla.editTextItemMessage("dialog", "But this is the first that a hero existed ...");
                                    break;
                                case 7:
                                    _sdla.editTextItemMessage("dialog", "Fortunatly, GRIZNIAK thinks that hero is you  ...");
                                    break;
                                case 8:
                                    _sdla.editTextItemMessage("dialog", "We will use this advantage to inspire fear  ...");
                                    break;
                                case 9:
                                    _sdla.editTextItemMessage("dialog", "You should be able to find him in the next room over  ...");
                                    break;
                                case 10:
                                    _sdla.editTextItemMessage("dialog", "Use this computer terminal to open the door.  ...");
                                    break;
                                case 11:
                                    _sdla.editTextItemMessage("dialog", "GOOD LUCK!!!");
                                    break;
                                case 12:
                                    allowStoryStep = false;
                                    _sdla.editTextItemVisibility("dialog", false);
                                    allowFinalComputerUse = true;
                                    break;
                                default:
                                    std::cout << "If you are reading this Josh messed up on level two. Fail him.\n";
                                    break;
                            }
                            spaceTap++; // Advance the dialog sequence
                        }
                        else
                        {
                            // Combat detection!!!
                            // find which way they are facing, and collision detect AOE against potential enemies
                            SDL_Rect hit = attackInFacingDirection(human);
                            if(checkForItemObjectColliding(hit) && allowSecondRoomCollisions)
                            {
                                /*
                                    Check for damage to enemy
                                 
                                 */
                                if(itemThatCollidedWithPlayer == '1')
                                {
                                    if(human.image == "sprites/male_basic_dagger.png" || human.image == "sprites/male_armor_dagger.png" ||
                                       human.image == "sprites/female_basic_dagger.png" || human.image == "sprites/female_armor_dagger.png" ||
                                       human.image == "sprites/male_basic.png" || human.image == "sprites/female_basic.png")
                                    {
                                        alienScientist1.health -= 3;
                                    }
                                    else{
                                        alienScientist1.health -= 1;
                                    }
                                }
                                // ALien 2
                                if(itemThatCollidedWithPlayer == '2')
                                {
                                    if(human.image == "sprites/male_basic_dagger.png" || human.image == "sprites/male_armor_dagger.png" ||
                                       human.image == "sprites/female_basic_dagger.png" || human.image == "sprites/female_armor_dagger.png" ||
                                       human.image == "sprites/male_basic.png" || human.image == "sprites/female_basic.png")
                                    {
                                        alienScientist2.health -= 3;
                                    }
                                    else{
                                        alienScientist2.health -= 1;
                                    }
                                }
                                
                                //alienGuard1
                                
                                // ALien 2
                                if(itemThatCollidedWithPlayer == '3')
                                {
                                    if(human.image == "sprites/male_basic_dagger.png" || human.image == "sprites/male_armor_dagger.png" ||
                                       human.image == "sprites/female_basic_dagger.png" || human.image == "sprites/female_armor_dagger.png" ||
                                       human.image == "sprites/male_basic.png" || human.image == "sprites/female_basic.png")
                                    {
                                        alienGuard1.health -= 2;
                                    }
                                    else{
                                        alienGuard1.health -= 1;
                                    }
                                }
                                
                                // Hit inst after alien death
                                if( (alienScientist1.health <= 0) && (alienScientist2.health <= 0) &&
                                    (alienGuard1.health <= 0) && (itemThatCollidedWithPlayer == '4') )
                                {
                                    _sdla.editTextItemMessage("userInfo", "Get to the next room! ");
                                }
                                // Hit inst2 after alien death
                                if( (alienScientist1.health <= 0) && (alienScientist2.health <= 0) &&
                                   (alienGuard1.health <= 0) && (itemThatCollidedWithPlayer == '5') )
                                    
                                {
                                    _sdla.editTextItemMessage("userInfo", "This apparently does nothing..");
                                }
                                // Hit machine after alien death
                                if( (alienScientist1.health <= 0) && (alienScientist2.health <= 0) &&
                                   (alienGuard1.health <= 0) && (itemThatCollidedWithPlayer == '6') )
                                {
                                    _sdla.editTextItemMessage("userInfo", "Door opened. YAY!");
                                    _sdla.freeScreenItem("door4");
                                }
                                
                                // Hit machine after alien death
                                if( (alienScientist1.health <= 0) && (alienScientist2.health <= 0) &&
                                   (alienGuard1.health <= 0) && (itemThatCollidedWithPlayer == '7') )
                                {
                                    _sdla.editTextItemVisibility("dialog", true);
                                    allowStoryStep = true;
                                    spaceTap = 1;
                                    checkForNosTalk = false;
                                }
                                
                                // Hit machine after alien death
                                if( (alienScientist1.health <= 0) && (alienScientist2.health <= 0) &&
                                   (alienGuard1.health <= 0) && (itemThatCollidedWithPlayer == '8') )
                                {
                                    _sdla.editTextItemMessage("userInfo", "You can now exit the room.");
                                }
                                
                                /*
                                 
                                        Check for enemy death
                                 
                                 */
                                if(alienScientist1.health <= 0)
                                {
                                    _sdla.editScreenItemSetVisibility("alienScientist1", false);
                                    _sdla.freeScreenItem("door3");
                                }
                                
                                // Check for alien 2 death
                                if(alienScientist2.health <= 0)
                                {
                                    _sdla.editScreenItemSetVisibility("alienScientist2", false);
                                    _sdla.freeScreenItem("door5");
                                    checkForNosTalk = true;
                                }
                                
                                // Check for alien guard death
                                if(alienGuard1.health <= 0)
                                {
                                    _sdla.editScreenItemSetVisibility("alienGuard1", false);
                                    _sdla.freeScreenItem("door2");
                                }
                            }
                        }
                    }
                    else if(_sdla._event.key.keysym.sym == SDLK_ESCAPE)
                    {
                        exit(0);
                        break;
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
