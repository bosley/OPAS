//
//  GameLevelThree.cpp
//  Game_321
//
//  Created by Josh Bosley on 11/4/16.
//  Copyright © 2016 Josh Bosley. All rights reserved.
//

#define FIRE_RATE 4
#define SHOW_RIGHT_TRIANGLES 0 // Set to 1 to see the lines
#define THIS_IS_THE_LAST_LEVEL 1 // Mark if this is the last level or if a transition will come out of this


#include "GameLevelThree.hpp"

GameLevelThree::GameLevelThree(humanoid &human)
{
    // Set human stuff
    human.x = 10;
    human.y = GAME_SCREEN_HEIGHT/2 - 64;
    human.spriteRow = 3*64; human.spriteColumn = 0;
    doHandleSpriteMovement = true;
    playerCollided = false;
    numberOfCollisions = 0;
    lastDetectedCollision = '0';
    textColor = {255,255,255};
    playerLastX = 0; playerLastY = 0;
    finalEventStage = 0;
    finalEvent = false;
    finalDialogComplete = false;

    // Load level images and text
    _sdla.setupSDL(GAME_SCREEN_HEIGHT, GAME_SCREEN_WIDTH, "Loomans!!!! - Level 3");
    
    // Load tut background
    _sdla.createScreenItem("bg", "backgrounds/level2bg.png",
                           0, 0, GAME_SCREEN_HEIGHT, GAME_SCREEN_WIDTH, 0, 0, GAME_SCREEN_HEIGHT, GAME_SCREEN_WIDTH);
    
    // Move to the approp spot of the image
    _sdla.editScreenItemSourceXY("bg", GAME_SCREEN_WIDTH*2, 0);
    
    // Information bar
    _sdla.createScreenItem("bar", "overlays/bar.png", 0, 0, 600, 800, 0, 0, GAME_SCREEN_HEIGHT, GAME_SCREEN_WIDTH);
    
    // Dialog box
    _sdla.createTextItem("dialog", "Hooman! You dare challenge me? ...",
                         "Arial.ttf", textColor, GAME_SCREEN_WIDTH/2 - 200 , 50, 50, GAME_SCREEN_WIDTH/2);
    
    // Boss health
    _sdla.createScreenItem("bossHealth", "sprites/boss_health.png", 0, 0, 100, 300, GAME_SCREEN_WIDTH-310, GAME_SCREEN_HEIGHT-50, 50, 300);
    
    // Player
    _sdla.createScreenItem("player", human.image, human.spriteRow, human.spriteColumn, 64, 64, human.x, human.y, 64, 64);
    
    // Create the boss
    humanoid boss;
    boss.image = "sprites/boss.png";
    boss.gender = true;
    boss.health = 90; // Should take 150 hits with a knife, or 150 punches
    boss.spriteRow = 3;
    boss.spriteColumn = 64*2;;
    boss.x = GAME_SCREEN_WIDTH/2-64;
    boss.y = GAME_SCREEN_HEIGHT/2 - 128;
    boss.itemName = "boss";
    
    // Create the boss man on the screen
    _sdla.createScreenItem(boss.itemName, boss.image,
                           boss.spriteRow, boss.spriteColumn, 64, 64, boss.x, boss.y, 128, 128);
    
    // Add the boss to the list of bipeds
    bipeds.push_back(boss);
    
    // Create Pylons and make invisible!
    _sdla.createScreenItem("pylon1", "sprites/pylon.png", 0, 0, 100, 50,
                           100, 20, 100, 50);
    _sdla.createScreenItem("pylon2", "sprites/pylon.png", 0, 0, 100, 50, 100,
                           GAME_SCREEN_HEIGHT-220, 100, 50);
    _sdla.createScreenItem("pylon3", "sprites/pylon.png", 0, 0, 100, 50,
                           GAME_SCREEN_WIDTH - 160 , 20, 100, 50);
    _sdla.createScreenItem("pylon4", "sprites/pylon.png", 0, 0, 100, 50,
                           GAME_SCREEN_WIDTH - 160, GAME_SCREEN_HEIGHT-220, 100, 50);
    _sdla.editScreenItemSetVisibility("pylon1", false);
    _sdla.editScreenItemSetVisibility("pylon2", false);
    _sdla.editScreenItemSetVisibility("pylon3", false);
    _sdla.editScreenItemSetVisibility("pylon4", false);
    
    
    // Create inventory screen
    _sdla.createScreenItem("inventory", "overlays/inv.png", 0, 0, 600, 800, 0, 0, 600, 800);
    
    // Toggle it off
    _sdla.editScreenItemSetVisibility("inventory", false);
    
    // Update hearts
    updateHeartDisplay(human);
    
    // Walk to initialize biped stuff, and put into position
    bipedController.walkSprite('r', human);
    
    // Run the map
    levelMainLoop(human);
}

SDL_Rect GameLevelThree::attackInFacingDirection(humanoid &attacker)
{
    SDL_Rect areaOfEffect; areaOfEffect.h = 64; areaOfEffect.w = 64;
    
    // For debugging
       _sdla.freeScreenItem("hitbox");
    
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
        _sdla.createScreenItem("hitbox", "sprites/blank.png", 0, 0, 64, 64, areaOfEffect.x, areaOfEffect.y, 64, 64);
    return areaOfEffect;
}

void GameLevelThree::performBounceBack(humanoid &h)
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

int GameLevelThree::checkForItemObjectColliding(SDL_Rect item)
{
    // Check to see if an item is colliding with another.lastDetectedCollision
    //   lastDetectedCollision = '1'; // Indicates that another sprite ran into the given item

    
    for(int i = 0; i < bipeds.size(); i++)
    {
        SDL_Rect b = _sdla.getScreenItemDestinationRectangleData(bipeds[i].itemName);
        if(((item.x > b.x) && (item.x < b.x+(b.w/2))) && ((item.y > b.y) && (item.y < b.y+b.h)))
        {
            lastDetectedCollision = '1';
            return i+1;
        }
    }


    
    // Check everything first. if you get here, then nothing collided
    return 0;
}

void GameLevelThree::updateHeartDisplay(humanoid &h)
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
    }

}

void GameLevelThree::updateBibeds(humanoid &human)
{
    /*
     
            Update player
     
     */
    // Check for object colliding with human
    // Update Health
    
    
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
        if(lastDetectedCollision == '1')
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
    
    // Check for human collision against pylons (running into them)
    if(_sdla.checkForCircleCircleCollision("player", "pylon1") && pylonWatch
       && _sdla.getScreenItemVisibility("pylon1"))
    {
        performBounceBack(human);
    }
    if(_sdla.checkForCircleCircleCollision("player", "pylon2") && pylonWatch
       && _sdla.getScreenItemVisibility("pylon2"))
    {
        performBounceBack(human);
    }
    if(_sdla.checkForCircleCircleCollision("player", "pylon3") && pylonWatch
       && _sdla.getScreenItemVisibility("pylon3"))
    {
        performBounceBack(human);
    }
    if(_sdla.checkForCircleCircleCollision("player", "pylon4") && pylonWatch
       && _sdla.getScreenItemVisibility("pylon4"))
    {
        performBounceBack(human);
    }
    
    _sdla.editScreenItemDestinationXY("player", (int)human.x, (int)human.y);
    
    
    
    
    /*
     
            Update NPCs
     
     
     */
    for(int i = 0; i < bipeds.size(); i++)
    {
        // Check for object colliding with npc
        // Update Health
        if(bipeds[i].health > 0)
        {
            // KILL THE BIPED!!
            
            if(bipeds[i].itemName == "boss")
            {
                if(bipeds[i].health == 30 )
                {
                    _sdla.editScreenItemSourceXY("bossHealth", 0, 200);
                }
                else if(bipeds[i].health == 60 )
                {
                    _sdla.editScreenItemSourceXY("bossHealth", 0, 100);
                }
            }
            else
            {
                _sdla.freeScreenItem(bipeds[i].itemName);
            }
        }
        else
        {
            if(bipeds[i].itemName == "boss" && !finalEvent)
            {
                _sdla.freeScreenItem("bossHealth");
                finalEvent = true;
            }
        }
        
        // Update Location
        // Update sprite location
        _sdla.editScreenItemDestinationXY(bipeds[i].image, (int)bipeds[i].x, (int)bipeds[i].y);
    }
}

void GameLevelThree::levelMainLoop(humanoid &human)
{
    unsigned fwState = 0;
    bool fireWheel = false; // The circle flames of DEATH!
    bool showingInv = false;
    int fireWheelDelta = 500;
    unsigned dialogIndex = 0;
    doHandleSpriteMovement = false;
    bool dialog = true, finalDialog = false;
    int bossLocation = -1;
    
    clock_t p1Start = 0, p2Start = 0, p3Start = 0, p4Start = 0;
    
    // Pylons that control bringing down the fire wheel
    pylon1 = false; pylon2 = false; pylon3 = false; pylon4 = false;
    pStage1 = 0; pStage2 = 0; pStage3 = 0; pStage4 = 0;
    pylonWatch = false; // Checks for event of all pylons being destroyed
    pylonDestCount = 0;
    
    std::vector<projectile> shots;
    int xOrY = rand() % 2;
    
    while(_sdla._isRunning)
    {
        xOrY = rand() % 2;
        
        // Update the people on the screen
        updateBibeds(human);
        
        // Render everything except 'color' for bg use
        _sdla.updateScreen(true, true, false);

        // Update the heart display for health
        updateHeartDisplay(human);
        
        // Check for death
        if(human.health <= 0)
        {
            human.win = false;
            human.lose = true;
            return;
        }
        
        if(finalEvent)
        {
            dialog = true;
        }
        
        // Find the boss location
        if(bossLocation == -1)
            for(int i = 0; i < bipeds.size(); i++)
            {
                if(bipeds[i].itemName == "boss")
                    bossLocation = i;
            }
        
        
        if(finalDialogComplete)
        {
            switch(finalEventStage)
            {
                case 0:
                    _sdla.editScreenItemSourceXY("boss", 0, 20*64);
                    bipeds[bossLocation].speedDelta = 0;
                    break;
                case 1:
                    if(bipeds[bossLocation].speedDelta == 200 )
                    {
                        _sdla.editScreenItemSourceXY("boss", 64, 20*64);
                        bipeds[bossLocation].speedDelta = 0;
                        finalEventStage++;
                    }
                    else
                        bipeds[bossLocation].speedDelta++;
                    break;
                case 2:
                    if(bipeds[bossLocation].speedDelta == 200 )
                    {
                        _sdla.editScreenItemSourceXY("boss", 128, 20*64);
                        bipeds[bossLocation].speedDelta = 0;
                        finalEventStage++;
                    }
                    else
                        bipeds[bossLocation].speedDelta++;
                    break;
                case 3:
                    if(bipeds[bossLocation].speedDelta == 200 )
                    {
                        _sdla.editScreenItemSourceXY("boss", 128*2, 20*64);
                        bipeds[bossLocation].speedDelta = 0;
                        finalEventStage++;
                    }
                    else
                        bipeds[bossLocation].speedDelta++;
                    break;
                case 4:
                    if(bipeds[bossLocation].speedDelta == 200 )
                    {
                        _sdla.editScreenItemSourceXY("boss", 128*3, 20*64);
                        bipeds[bossLocation].speedDelta = 0;
                        finalEventStage++;
                    }
                    else
                        bipeds[bossLocation].speedDelta++;
                    break;
                default:
                    break;
            }
        }
        
        /*
                            NEED TO COMPLETE PYLON FUNCTIONALITY. ALL WILL SHOOT WITH A DEGREE OF ACCURACY
                            AT THE PLAYER. AS THE PLAYER HITS. THE SOURCE CHANGES UNTIL IT 'EXPLODES' 
         
                            ONCE ALL PYLONS ARE KILLED. FIRE SHIELD GOES AWAY FOR x seconds

         */
        
        
        /*
         
            Here we have to handle the trajectory of each bullet, so we draw a grid using the pylons x, and y as the side
            of the triangle created between the
         
         */
        // Center of each pylon
        SDL_Rect cp1 = _sdla.getScreenItemDestinationRectangleData("pylon1");
        SDL_Rect cp2 = _sdla.getScreenItemDestinationRectangleData("pylon2");
        SDL_Rect cp3 = _sdla.getScreenItemDestinationRectangleData("pylon3");
        SDL_Rect cp4 = _sdla.getScreenItemDestinationRectangleData("pylon4");
        SDL_Rect player = _sdla.getScreenItemDestinationRectangleData("player");
        
        SDL_Point pylon1Center , pylon2Center, pylon3Center, pylon4Center, playerCenter;
        pylon1Center.x = (cp1.x + cp1.w/2);
        pylon1Center.y = (cp1.y + cp1.h/2);
        pylon2Center.x = (cp2.x + cp2.w/2);
        pylon2Center.y = (cp2.y + cp2.h/2);
        pylon3Center.x = (cp3.x + cp3.w/2);
        pylon3Center.y = (cp3.y + cp3.h/2);
        pylon4Center.x = (cp4.x + cp4.w/2);
        pylon4Center.y = (cp4.y + cp4.h/2);
        playerCenter.x = (player.x + player.w/2);
        playerCenter.y = (player.y + player.h/2);
        
        if(SHOW_RIGHT_TRIANGLES == 1)
        {
            _sdla.freeLineSegment("line0"); _sdla.freeLineSegment("line1"); _sdla.freeLineSegment("line2");
            _sdla.freeLineSegment("line3"); _sdla.freeLineSegment("line4"); _sdla.freeLineSegment("line5");
            _sdla.freeLineSegment("line6"); _sdla.freeLineSegment("line7");
            _sdla.createLineSegment("line0", fPoint(pylon1Center.x, pylon1Center.y),
                                fPoint(playerCenter.x, playerCenter.y));
            _sdla.createLineSegment("line1", fPoint(pylon1Center.x, playerCenter.y),
                                fPoint(playerCenter.x, playerCenter.y));
            _sdla.createLineSegment("line2", fPoint(pylon3Center.x, pylon3Center.y),
                                fPoint(playerCenter.x, playerCenter.y));
            _sdla.createLineSegment("line3", fPoint(pylon3Center .x, playerCenter.y),
                                fPoint(playerCenter.x, playerCenter.y));
            _sdla.createLineSegment("line4", fPoint(pylon2Center.x, pylon2Center.y),
                                fPoint(playerCenter.x, playerCenter.y));
            _sdla.createLineSegment("line5", fPoint(pylon2Center.x, playerCenter.y),
                                fPoint(playerCenter.x, playerCenter.y));
            _sdla.createLineSegment("line6", fPoint(pylon4Center.x, pylon4Center.y),
                                fPoint(playerCenter.x, playerCenter.y));
            _sdla.createLineSegment("line7", fPoint(pylon4Center.x, playerCenter.y),
                                fPoint(playerCenter.x, playerCenter.y));
        }
        
        // Update all the shots!
        for(int i = 0; i < shots.size(); i++ )
        {
            // Get teh distance from the pylon to orb and from teh orb to the human
            // the idea here is to adjust the path to ensure that the projectile
            // travels in the correct direction. The path is formed fiunky, so this is my work around
            double orb_human = pow((playerCenter.x - shots[i].path[0].x), 2);
            orb_human += pow((playerCenter.y - shots[i].path[0].y), 2);
            double orb_pylon = pow((human.x - shots[i].path[0].x), 2);
            orb_pylon += pow((human.y - shots[i].path[0].y), 2);
            
            // If its going in the wrong direction, then flip the vector.
            if(orb_pylon > orb_human && (!shots[i].lolflag))
            {
                shots[i].lolflag = true;
                std::reverse(shots[i].path.begin(), shots[i].path.end());
            }
            
            // Move the shot around
            if(shots[i].pindex < shots[i].path.size())
            {
                _sdla.editScreenItemDestinationXY(shots[i].name,
                                                  shots[i].path[shots[i].pindex].x, shots[i].path[shots[i].pindex++].y);
                
                // See if the orb hits the player and deduct the approps damage.
                if(_sdla.checkForCirclePointCollision("player", shots[i].name))
                {
                    _sdla.freeScreenItem(shots[i].name);
                    human.health -= 0.5;
                }
            }
            else
            {
                _sdla.freeScreenItem(shots[i].name);
            }
        }
        
        // Show pylons at their specific stage
        if(pylon1)
        {
            if(pStage1 == 0)
            {
                _sdla.editScreenItemSourceXY("pylon1", 0, 0);
            }
            else if (pStage1 == 1)
            {
                _sdla.editScreenItemSourceXY("pylon1", 0, 100);
            }
            else if (pStage1 == 2)
            {
                _sdla.editScreenItemSourceXY("pylon1", 0, 200);
            }
            // THIS GOES INSIDE PYLON 1 AREA
            // Check to see if its time for the pylon to fire
    
             // SHOOT ORB 1
            if( ((double)(clock() - p1Start)/CLOCKS_PER_SEC) > FIRE_RATE )
            {
                p1Start = clock(); // Reset fire clock
                SDL_Rect temp = _sdla.getScreenItemDestinationRectangleData("pylon1");
                SDL_Rect temp2 = _sdla.getScreenItemDestinationRectangleData("player");
                double sx = temp.x + (temp.w/2);
                double sy = temp.y + (temp.h/2);
                double tx = temp2.x + (temp2.w/2);
                double ty = temp2.y + (temp2.h/2);
                int accuracy = rand() % 10;         // This will control how close the shot is to the player
                int highOrLow = rand() % 2;        // This controls if its high or low relative to the player
                
                if(highOrLow == 1)
                {
                    // High (Add)
                    if(xOrY == 1)
                    {
                        // X
                        tx += accuracy;
                    }
                    else
                    {
                        // Y
                        ty += accuracy;
                    }
                }
                else
                {
                    // Low (Sub)
                    if(xOrY == 1)
                    {
                        // X
                        tx -= accuracy;
                    }
                    else
                    {
                        // Y
                        ty -= accuracy;
                    }
                }
                
                // Create a projectile item and add it to the shots fired
                std::string newProjectileName = "PROJECTILE_" + std::to_string(shots.size());                   // Create
                shots.push_back(projectile( newProjectileName, "sprites/orb.png", sx, sy, 1.00, tx, ty, sx, sy));
                _sdla.createScreenItem( newProjectileName, "sprites/orb.png", 0, 0, 50, 50, sx, sy, 50, 50);
            }
            _sdla.editScreenItemSetVisibility("pylon1", true);
        }
        if(pylon2)
        {
            if(pStage2 == 0)
            {
                _sdla.editScreenItemSourceXY("pylon2", 0, 0);
            }
            else if (pStage2 == 1)
            {
                _sdla.editScreenItemSourceXY("pylon2", 0, 100);
            }
            else if (pStage2 == 2)
            {
                _sdla.editScreenItemSourceXY("pylon2", 0, 200);
            }
            
            // SHOOT ORB 2
            if( ((double)(clock() - p2Start)/CLOCKS_PER_SEC) > FIRE_RATE )
            {
                p2Start = clock(); // Reset fire clock
                SDL_Rect temp = _sdla.getScreenItemDestinationRectangleData("pylon2");
                SDL_Rect temp2 = _sdla.getScreenItemDestinationRectangleData("player");
                double sx = temp.x + (temp.w/2);
                double sy = temp.y + (temp.h/2);
                double tx = temp2.x + (temp2.w/2);
                double ty = temp2.y + (temp2.h/2);
                int accuracy = rand() % 10;         // This will control how close the shot is to the player
                int highOrLow = rand() % 2;        // This controls if its high or low relative to the player
                
                if(highOrLow == 1)
                {
                    // High (Add)
                    if(xOrY == 1)
                    {
                        // X
                        tx += accuracy;
                    }
                    else
                    {
                        // Y
                        ty += accuracy;
                    }
                }
                else
                {
                    // Low (Sub)
                    if(xOrY == 1)
                    {
                        // X
                        tx -= accuracy;
                    }
                    else
                    {
                        // Y
                        ty -= accuracy;
                    }
                }
                
                // Create a projectile item and add it to the shots fired
                std::string newProjectileName = "PROJECTILE_" + std::to_string(shots.size());                   // Create
                shots.push_back(projectile( newProjectileName, "sprites/orb.png", sx, sy, 1.00, tx, ty, sx, sy));
                _sdla.createScreenItem( newProjectileName, "sprites/orb.png", 0, 0, 50, 50, sx, sy, 50, 50);
            }
            _sdla.editScreenItemSetVisibility("pylon2", true);
        }
        if(pylon3)
        {
            if(pStage3 == 0)
            {
                _sdla.editScreenItemSourceXY("pylon3", 0, 0);
            }
            else if (pStage3 == 1)
            {
                _sdla.editScreenItemSourceXY("pylon3", 0, 100);
            }
            else if (pStage3 == 2)
            {
                _sdla.editScreenItemSourceXY("pylon3", 0, 200);
            }
            
            // SHOOT ORB 3
            if( ((double)(clock() - p3Start)/CLOCKS_PER_SEC) > FIRE_RATE )
            {
                p3Start = clock(); // Reset fire clock
                SDL_Rect temp = _sdla.getScreenItemDestinationRectangleData("pylon3");
                SDL_Rect temp2 = _sdla.getScreenItemDestinationRectangleData("player");
            
                double sx = temp.x + (temp.w/2);
                double sy = temp.y + (temp.h/2);
                double tx = temp2.x + (temp2.w/2);
                double ty = temp2.y + (temp2.h/2);
                
                int accuracy = rand() % 10;         // This will control how close the shot is to the player
                int highOrLow = rand() % 2;        // This controls if its high or low relative to the player
                
                if(highOrLow == 1)
                {
                    // High (Add)
                    if(xOrY == 1)
                    {
                        // X
                        tx += accuracy;
                    }
                    else
                    {
                        // Y
                        ty += accuracy;
                    }
                }
                else
                {
                    // Low (Sub)
                    if(xOrY == 1)
                    {
                        // X
                        tx -= accuracy;
                    }
                    else
                    {
                        // Y
                        ty -= accuracy;
                    }
                }
                
                // Create a projectile item and add it to the shots fired
                std::string newProjectileName = "PROJECTILE_" + std::to_string(shots.size());                   // Create
                shots.push_back(projectile( newProjectileName, "sprites/orb.png", sx, sy, 1.00, tx, ty, sx, sy));
                _sdla.createScreenItem( newProjectileName, "sprites/orb.png", 0, 0, 50, 50, sx, sy, 50, 50);
            }
            _sdla.editScreenItemSetVisibility("pylon3", true);
        }
        if(pylon4)
        {
            if(pStage4 == 0)
            {
                _sdla.editScreenItemSourceXY("pylon4", 0, 0);
            }
            else if (pStage4 == 1)
            {
                _sdla.editScreenItemSourceXY("pylon4", 0, 100);
            }
            else if (pStage4 == 2)
            {
                _sdla.editScreenItemSourceXY("pylon4", 0, 200);
            }

            // SHOOT ORB 4
            if( ((double)(clock() - p4Start)/CLOCKS_PER_SEC) > FIRE_RATE )
            {
                p4Start = clock(); // Reset fire clock
                SDL_Rect temp = _sdla.getScreenItemDestinationRectangleData("player");
                SDL_Rect temp2 = _sdla.getScreenItemDestinationRectangleData("pylon4");
                double sx = temp.x + (temp.w/2);
                double sy = temp.y + (temp.h/2);
                double tx = temp2.x + (temp2.w/2);
                double ty = temp2.y + (temp2.h/2);
                int accuracy = rand() % 10;         // This will control how close the shot is to the player
                int highOrLow = rand() % 2;        // This controls if its high or low relative to the player
                
                if(highOrLow == 1)
                {
                    // High (Add)
                    if(xOrY == 1)
                    {
                        // X
                        tx += accuracy;
                    }
                    else
                    {
                        // Y
                        ty += accuracy;
                    }
                }
                else
                {
                    // Low (Sub)
                    if(xOrY == 1)
                    {
                        // X
                        tx -= accuracy;
                    }
                    else
                    {
                        // Y
                        ty -= accuracy;
                    }
                }
                
                // Create a projectile item and add it to the shots fired
                std::string newProjectileName = "PROJECTILE_" + std::to_string(shots.size());                   // Create
                shots.push_back(projectile( newProjectileName, "sprites/orb.png", sx, sy, 1.00, tx, ty, sx, sy));
                _sdla.createScreenItem( newProjectileName, "sprites/orb.png", 0, 0, 50, 50, sx, sy, 50, 50);
            }
            _sdla.editScreenItemSetVisibility("pylon4", true);
        }
        
        // If all the pylons are gone, disable the fire shield
        if(pylonDestCount == 4 && pylonWatch)
        {
            fwState = 0;
            fireWheel = false;
            pylonDestCount = 0;
            pylonWatch = false;
            _sdla.freeScreenItem("fireBall1"); _sdla.freeScreenItem("fireBall2");
            _sdla.freeScreenItem("fireBall3"); _sdla.freeScreenItem("fireBall4");
            _sdla.freeScreenItem("fireBall5"); _sdla.freeScreenItem("fireBall7");
            _sdla.freeScreenItem("fireBall6"); _sdla.freeScreenItem("fireBall8");
        }
        
        // Display and handle the firewheel
        if(fireWheel && (fireWheelDelta >= 125))
        {
            // Present and handle fire wheel
            fireWheelDelta = 0;
            switch(fwState)
            {
                case 0:
                    fwState++;
                    ///*
                    _sdla.freeScreenItem("fireBall1"); _sdla.freeScreenItem("fireBall2");
                    _sdla.freeScreenItem("fireBall3"); _sdla.freeScreenItem("fireBall4");
                    _sdla.createScreenItem("fireBall1", "sprites/orb.png", 0, 0, 50, 50,
                                           (GAME_SCREEN_WIDTH/2) - 150, GAME_SCREEN_HEIGHT/2-50, 50, 50);
                    _sdla.createScreenItem("fireBall2", "sprites/orb.png", 0, 0, 50, 50,
                                           (GAME_SCREEN_WIDTH/2) - 25, GAME_SCREEN_HEIGHT/2 + 50, 50, 50);
                    _sdla.createScreenItem("fireBall3", "sprites/orb.png", 0, 0, 50, 50,
                                           (GAME_SCREEN_WIDTH/2) + 80, GAME_SCREEN_HEIGHT/2-50, 50, 50);
                    _sdla.createScreenItem("fireBall4", "sprites/orb.png", 0, 0, 50, 50,
                                           (GAME_SCREEN_WIDTH/2) - 25, GAME_SCREEN_HEIGHT/2 - 180, 50, 50);
                    break;
                case 1:
                    fwState++;
                    _sdla.createScreenItem("fireBall5", "sprites/orb.png", 0, 0, 50, 50,
                                           (GAME_SCREEN_WIDTH/2) - 100, GAME_SCREEN_HEIGHT/2, 50, 50);
                    _sdla.createScreenItem("fireBall6", "sprites/orb.png", 0, 0, 50, 50,
                                           (GAME_SCREEN_WIDTH/2) + 50, GAME_SCREEN_HEIGHT/2, 50, 50);
                    _sdla.createScreenItem("fireBall7", "sprites/orb.png", 0, 0, 50, 50,
                                           (GAME_SCREEN_WIDTH/2) - 100, GAME_SCREEN_HEIGHT/2 -75, 50, 50);
                    _sdla.createScreenItem("fireBall8", "sprites/orb.png", 0, 0, 50, 50,
                                           (GAME_SCREEN_WIDTH/2) + 50, GAME_SCREEN_HEIGHT/2 - 75, 50, 50);
                    break;
                case 2:
                    fwState++;
                    _sdla.freeScreenItem("fireBall1"); _sdla.freeScreenItem("fireBall2");
                    _sdla.freeScreenItem("fireBall3"); _sdla.freeScreenItem("fireBall4");
                    _sdla.freeScreenItem("fireBall5"); _sdla.freeScreenItem("fireBall7");
                    _sdla.freeScreenItem("fireBall6"); _sdla.freeScreenItem("fireBall8");
                    _sdla.createScreenItem("fireBall1", "sprites/orb.png", 0, 0, 50, 50,
                                           (GAME_SCREEN_WIDTH/2) - 100, GAME_SCREEN_HEIGHT/2+50, 50, 50);
                    _sdla.createScreenItem("fireBall2", "sprites/orb.png", 0, 0, 50, 50,
                                           (GAME_SCREEN_WIDTH/2) + 50, GAME_SCREEN_HEIGHT/2 + 50, 50, 50);
                    _sdla.createScreenItem("fireBall3", "sprites/orb.png", 0, 0, 50, 50,
                                           (GAME_SCREEN_WIDTH/2) - 100, GAME_SCREEN_HEIGHT/2 -120, 50, 50);
                    _sdla.createScreenItem("fireBall4", "sprites/orb.png", 0, 0, 50, 50,
                                           (GAME_SCREEN_WIDTH/2) + 50, GAME_SCREEN_HEIGHT/2 - 120, 50, 50);
                    break;
                case 3:
                    fwState = 0;
                    break;
            }
            
            // Check for fire ball collision
            for(int i = 1; i <= 6; i++)
            {
                // Grab each rect by name and check for collision
                std::string current = ("fireBall" + std::to_string(i));
                if(_sdla.checkForSquarePointCollision("player", current))
                {
                    human.health -=2;
                }
            }
        }
        if(fireWheelDelta < 500)
            fireWheelDelta++;
        
        
        
        /*
         
         
                    MOVEMENT
         
         
         */
        
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
                    SDL_Rect tempRect = _sdla.getScreenItemDestinationRectangleData("player");
                    if(_sdla._event.key.keysym.sym == SDLK_w)            // U
                    {
                        if(doHandleSpriteMovement)
                            bipedController.walkSprite('u', human);
                        playerLastX = tempRect.x; playerLastY = tempRect.y;
                    }
                    else if(_sdla._event.key.keysym.sym == SDLK_s)       // D
                    {
                        if(doHandleSpriteMovement)
                            bipedController.walkSprite('d', human);
                        playerLastX = tempRect.x; playerLastY = tempRect.y;
                    }
                    else if(_sdla._event.key.keysym.sym == SDLK_a)       // L
                    {
                        if(doHandleSpriteMovement)
                            bipedController.walkSprite('l', human);
                        playerLastX = tempRect.x; playerLastY = tempRect.y;
                    }
                    else if(_sdla._event.key.keysym.sym == SDLK_d)       // R
                    {
                        if(doHandleSpriteMovement)
                            bipedController.walkSprite('r', human);
                        playerLastX = tempRect.x; playerLastY = tempRect.y;
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
                        // Check for dialog inc
                        if(dialog || finalDialog)
                        {
                            switch(dialogIndex)
                            {
                                case 0:
                                    _sdla.editTextItemMessage("dialog", "You escaped my guards, and now...");
                                    break;
                                case 1:
                                    _sdla.editTextItemMessage("dialog", "Now you dare fight me for control of the loomans?!! ...");
                                    break;
                                case 2:
                                    _sdla.editTextItemMessage("dialog", "THE HUBRIS!!");
                                    break;
                                case 3:
                                    _sdla.editTextItemMessage("dialog", "I will eat your flesh ...");
                                    break;
                                case 4:
                                    _sdla.editTextItemMessage("dialog", "and pick my teeth with your bones ...");
                                    break;
                                case 5:
                                    _sdla.editTextItemMessage("dialog", "GRRRRRRRAAAAHHHHH!!!!");
                                    break;
                                case 6:
                                    _sdla.editTextItemVisibility("dialog", false);
                                    dialog = false; finalDialog = false;
                                    doHandleSpriteMovement = true;
                                    pylon1 = true; pylon2 = true;
                                    pylon3 = true; pylon4 = true;
                                    pylonDestCount = 0;
                                    pylonWatch = true;
                                    fireWheel = true; // Enable the firewall!
                                    // Load the pylons, and begin sequence
                                    p1Start = clock(); p2Start = clock();
                                    p3Start = clock(); p4Start = clock();
                                    break;
                                case 7:
                                    if(THIS_IS_THE_LAST_LEVEL)
                                    {
                                        _sdla.editTextItemMessage("dialog", "AGGGGHHHHH NOOO!!!!");
                                        _sdla.editTextItemVisibility("dialog", true);
                                    }
                                    else
                                    {
                                        //
                                    }
                                    
                                    
                                    break;
                                case 8:
                                    if(THIS_IS_THE_LAST_LEVEL)
                                    {
                                        _sdla.editTextItemMessage("dialog", "How did you defeate my death rays?!?!");
                                    }
                                    else
                                    {
                                        //
                                    }
                                    break;
                                case 9:
                                    if(THIS_IS_THE_LAST_LEVEL)
                                    {
                                        _sdla.editTextItemMessage("dialog", "They were Tesla's design!");
                                    }
                                    else
                                    {
                                        //
                                    }
                                    break;
                                case 10:
                                    if(THIS_IS_THE_LAST_LEVEL)
                                    {
                                        _sdla.editTextItemMessage("dialog", "You... You must be an EDISON!");
                                    }
                                    else
                                    {
                                        //
                                    }
                                    break;
                                case 11:
                                    if(THIS_IS_THE_LAST_LEVEL)
                                    {
                                        human.lose = false;
                                        human.win = true;
                                        return;
                                    }
                                    else
                                    {
                                        //
                                    }
                                    break;
                                    
                                    
                                    
                                default:
                                    break;
                            }
                            dialogIndex++;
                        }
                        else
                        {
                            // Generate the hitbox
                            SDL_Rect aoe = attackInFacingDirection(human);
                            
                            // ATTACK!
                            // Get the index number of the biped that was hit by the player
                            int ret_item = checkForItemObjectColliding(aoe);
                            if(0 != (ret_item))
                            {
                                // If a biped is hit, ldc will be = '1' and ret_item will be the index of the biped + 1
                                if(lastDetectedCollision == '1')
                                {
                                    if(human.image == "sprites/male_basic_dagger.png" || human.image == "sprites/male_armor_dagger.png" ||
                                       human.image == "sprites/female_basic_dagger.png" || human.image == "sprites/female_armor_dagger.png" ||
                                       human.image == "sprites/male_basic.png" || human.image == "sprites/female_basic.png")
                                    {
                                        bipeds[ret_item-1].health -= 3;
                                        //std::cout << "HIT!\n";
                                    }
                                    else
                                    {
                                        bipeds[ret_item-1].health -= 1;
                                       // std::cout << "HIT!\n";
                                    }
                                }
                            }
                            
                            if(_sdla.checkForCircleCircleCollision("hitbox", "pylon1"))
                            {
                                pStage1++;
                                if (pStage1 > 2)
                                {
                                    pStage1 = 0;
                                    pylon1 = false;
                                    _sdla.editScreenItemSetVisibility("pylon1", false);
                                    
                                    if(pylonWatch)
                                        if(pylonDestCount <= 3)
                                            pylonDestCount++;
                                }
                            }
                            
                            if(_sdla.checkForCircleCircleCollision("hitbox", "pylon2"))
                            {
                                pStage2++;
                                if (pStage2 > 2)
                                {
                                    pStage2 = 0;
                                    pylon2 = false;
                                    _sdla.editScreenItemSetVisibility("pylon2", false);
                                    
                                    if(pylonWatch)
                                        if(pylonDestCount <= 3)
                                            pylonDestCount++;
                                }
                            }
                            
                            if(_sdla.checkForCircleCircleCollision("hitbox", "pylon3"))
                            {
                                pStage3++;
                                if (pStage3 > 2)
                                {
                                    pStage3 = 0;
                                    pylon3 = false;
                                    _sdla.editScreenItemSetVisibility("pylon3", false);
                                    
                                    if(pylonWatch)
                                        if(pylonDestCount <= 3)
                                            pylonDestCount++;
                                }
                            }
                            
                            if(_sdla.checkForCircleCircleCollision("hitbox", "pylon4"))
                            {
                                pStage4++;
                                if (pStage4 > 2)
                                {
                                    pStage4 = 0;
                                    pylon4 = false;
                                    _sdla.editScreenItemSetVisibility("pylon4", false);

                                    if(pylonWatch)
                                        if(pylonDestCount <= 3)
                                            pylonDestCount++;
                                }
                            }
                            
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



