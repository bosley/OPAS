//
//  GameLevelTutorial.cpp
//  Game_321
//
//  Created by Josh Bosley on 10/12/16.
//  Copyright © 2016 Josh Bosley. All rights reserved.
//

#include "GameLevelOne.hpp"

GameLevelOne::GameLevelOne(humanoid &human)
{
    // Init
    human.spriteRow = 11 * 64;
    human.spriteRow = 0;
    levelState = walkIn;
    textColor = {255,255,255};
    human.x = -10;
    human.y = GAME_SCREEN_HEIGHT/2 - 64;
    human.health = 6; // 2 per heart
    doHandleSpriteMovement = false;
    itemThatCollidedWithPlayer = 'N';
    
    // Load level images and text
    _sdla.setupSDL(GAME_SCREEN_HEIGHT, GAME_SCREEN_WIDTH, "Loomans!!!! - Level 1");
    
    // Load tut background
    _sdla.createScreenItem("bg", "backgrounds/background_red.png",
                           0, 0, GAME_SCREEN_HEIGHT, GAME_SCREEN_WIDTH, 0, 0, GAME_SCREEN_HEIGHT, GAME_SCREEN_WIDTH);
    
    // Information bar
    _sdla.createScreenItem("bar", "overlays/bar.png", 0, 0, 600, 800, 0, 0, GAME_SCREEN_HEIGHT, GAME_SCREEN_WIDTH);
    
    // Health
    _sdla.createScreenItem("heart1", "sprites/heart.png", 0, 0, 15, 16, 50, GAME_SCREEN_HEIGHT-30, 20, 20);
    _sdla.createScreenItem("heart2", "sprites/heart.png", 0, 0, 15, 16, 90, GAME_SCREEN_HEIGHT-30, 20, 20);
    _sdla.createScreenItem("heart3", "sprites/heart.png", 0, 0, 15, 16, 130, GAME_SCREEN_HEIGHT-30, 20, 20);
    
    // Teleport pad
    _sdla.createScreenItem("portal", "sprites/teleport.png", 0, 0, 100, 100,
                           (GAME_SCREEN_WIDTH/2)-50 , (GAME_SCREEN_HEIGHT/2)-30, 100, 100);
    _sdla.editScreenItemSetVisibility("portal", false);
    
    // Create player sprite
    _sdla.createScreenItem("player", human.image, human.spriteRow, human.spriteColumn, 64, 64, human.x, human.y, 64, 64);
    
    // Create aliens!
    
    // Guard 1
    alienGuardOne.image = "sprites/alien_female.png";
    alienGuardOne.gender = false;
    alienGuardOne.health = 3;
    alienGuardOne.spriteRow = 11 * 64;
    alienGuardOne.spriteColumn = 0;
    alienGuardOne.x = -74;
    alienGuardOne.y = GAME_SCREEN_HEIGHT/2 - 130;
    alienGuardOne.speedDelta = 40;
    alienGuardOne.moveSwitch = 'A';
    alienGuardOne.droppedItem = false;
    
    _sdla.createScreenItem("alien1", alienGuardOne.image,
                           alienGuardOne.spriteRow, alienGuardOne.spriteColumn, 64, 64, alienGuardOne.x, alienGuardOne.y, 64, 64);
    // The loot from killing alien 1
    _sdla.createScreenItem("knifeStar1", "sprites/knife_star.png",
                           0, 0, 50, 50, alienGuardOne.x, alienGuardOne.y, 50, 50);
    _sdla.editTextItemVisibility("knifeStar1", false);  // Hide it
    
    // Guard 2
    alienGuardTwo.image = "sprites/alien_female.png";
    alienGuardTwo.gender = false;
    alienGuardTwo.health = 3;
    alienGuardTwo.spriteRow = 11 * 64;
    alienGuardTwo.spriteColumn = 0;
    alienGuardTwo.x = -74;
    alienGuardTwo.y = GAME_SCREEN_HEIGHT/2 ;
    alienGuardTwo.speedDelta = 40;
    alienGuardTwo.moveSwitch = 'A';
    alienGuardTwo.droppedItem = false;
    
    _sdla.createScreenItem("alien2", alienGuardTwo.image,
                           alienGuardTwo.spriteRow, alienGuardTwo.spriteColumn, 64, 64, alienGuardTwo.x, alienGuardTwo.y, 64, 64);
    // The loot from killing alien 1
    _sdla.createScreenItem("armor", "sprites/armor_pickup.png",
                           0, 0, 50, 50, alienGuardOne.x, alienGuardOne.y, 50, 50);
    _sdla.editTextItemVisibility("armor", false);  // Hide it
    
    // BOSS
    boss.image = "sprites/boss.png";
    boss.gender = true;
    boss.health = 500;
    boss.spriteRow = 5 * 64;
    boss.spriteColumn = 0;
    boss.x = GAME_SCREEN_WIDTH/2;
    boss.y = GAME_SCREEN_HEIGHT/2 - 128;
    
    _sdla.createScreenItem("boss", boss.image,
                           boss.spriteRow, boss.spriteColumn, 64, 64, boss.x, boss.y, 128, 128);
    
    // Create inventory screen
    _sdla.createScreenItem("inventory", "overlays/inv.png", 0, 0, 600, 800, 0, 0, 600, 800);
    
    // Toggle it off
    _sdla.editScreenItemSetVisibility("inventory", false);
    
    _sdla.createTextItem("userInfo", "PRESS SPACE TO CONTINUE DIALOG", "Arial.ttf", textColor, GAME_SCREEN_WIDTH-400, GAME_SCREEN_HEIGHT-40, 40, 400);

    // Run the main loop
    levelMainLoop(human);
    
    // Clean up everything, and stop display before moving on
    _sdla.killSdlAssist();
}

// Circle-point collision detection
bool GameLevelOne::checkForPlayerHumanoidColliding(SDL_Rect p, SDL_Rect o)
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

bool GameLevelOne::checkForItemObjectColliding(SDL_Rect item)
{
    // Check humanoids
    SDL_Rect alienOne = _sdla.getScreenItemDestinationRectangleData("alien1");
    SDL_Rect alienTwo = _sdla.getScreenItemDestinationRectangleData("alien2");
    
    if( alienGuardOne.health > 0 && checkForPlayerHumanoidColliding(item, alienOne) )
    {
        itemThatCollidedWithPlayer = '1';
        return true;
    }
    if( alienGuardTwo.health > 0 && checkForPlayerHumanoidColliding(item, alienTwo) )
    {
        itemThatCollidedWithPlayer = '2';
        return true;
    }
    
    // Check other
    return false;
}

// Only for 64x64 humanoids
SDL_Rect GameLevelOne::attackInFacingDirection(humanoid &attacker)
{
    SDL_Rect areaOfEffect;
    areaOfEffect.h = 64;
    areaOfEffect.w = 64;
    
    // For debugging
  // _sdla.freeScreenItem("hitbox");
    
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
  //   _sdla.createScreenItem("hitbox", "sprites/hitbox.png", 0, 0, 64, 64, areaOfEffect.x, areaOfEffect.y, 64, 64);
    return areaOfEffect;
}

void GameLevelOne::levelMainLoop(humanoid &human)
{
    // Story control
    int spaceTap = 0;
    bool allowStoryStep = false;
    bool playerCollided = false;
    int numberOfCollisions = 0; // For pushing user away when in collision
    int portalSpeedDelta = 50;
    bool showingInv = false;
    
    // Player reset controlls
    SDL_Rect tempRect;
    int playerLastX = 0, playerLastY = 0;
    
    // Main loop
    while(_sdla._isRunning)
    {
        
        // Portal showing stage
        if(spaceTap >= 13)
        {
            if (portalSpeedDelta == 50)
            {
                SDL_Rect a = _sdla.getScreenItemSourceRectangleData("portalAn");
                if(a.x == 800)
                    a.x = 0;
                else
                    a.x += 200;
                    _sdla.editScreenItemSourceXY("portalAn", a.x, a.y);
                portalSpeedDelta = 0;
            }
            else
                portalSpeedDelta++;
        }
        
    // I know
    avoidClippingOnCollision:
        // Update player image and location
        _sdla.editScreenItemSourceXY("player", human.spriteColumn, human.spriteRow);
        _sdla.editScreenItemDestinationXY("player", (int)human.x, (int)human.y);
        
        // Update alien one image and location
        _sdla.editScreenItemSourceXY("alien1", alienGuardOne.spriteColumn, alienGuardOne.spriteRow);
        _sdla.editScreenItemDestinationXY("alien1", (int)alienGuardOne.x, (int)alienGuardOne.y);
        
        // Update alien two image and location
        _sdla.editScreenItemSourceXY("alien2", alienGuardTwo.spriteColumn, alienGuardTwo.spriteRow);
        _sdla.editScreenItemDestinationXY("alien2", (int)alienGuardTwo.x, (int)alienGuardTwo.y);
        
        // Update boss image and location
        _sdla.editScreenItemSourceXY("boss", boss.spriteColumn, boss.spriteRow);
        _sdla.editScreenItemDestinationXY("boss", (int)boss.x, (int)boss.y);
        
        // Check player sprite is colliding with aliens
        playerCollided = false;
        if(checkForItemObjectColliding(_sdla.getScreenItemDestinationRectangleData("player")))
        {
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
        
        // Render everything
        _sdla.updateScreen(true, true, true);
        
        switch(levelState)
        {
            case walkIn:
            {
                // Walk sprites to middle of screen
                if((int)human.x < GAME_SCREEN_WIDTH/2 - 164)
                {
                    bipedController.walkSprite('r', human);
                    bipedController.walkSprite('r', alienGuardOne);
                    bipedController.walkSprite('r', alienGuardTwo);
                    SDL_Delay(50);
                }
                else
                {
                    levelState = introDialog;
                    allowStoryStep = true;
                    _sdla.createTextItem("story1", "I am GRIZNIAK, the Leader of Loomans (L.O.L)",
                                         "Arial.ttf", textColor, 100, 20, 25, 600);
                }
                break;
            }
                
            case introDialog:
            {
                if(spaceTap == 8)
                {
                    levelState = exitBoss;
                }
                break;
            }
                
            case exitBoss:
            {
                // Make the boss exit
                if((int)boss.x < GAME_SCREEN_WIDTH-1 )
                {
                    // Passing in true at the end disables boundaries
                    bipedController.walkSprite('r', boss, true);
                    SDL_Delay(50);
                }
                else
                {
                    allowStoryStep = true;
                    _sdla.freeScreenItem("boss");
                    _sdla.editTextItemVisibility("story1", true);
                    _sdla.editTextItemMessage("story1", "[VOICE IN HEAD] : Now is your chance ...");
                    levelState = learnFight;
                }
                break;
            }
                
            case learnFight:
            {
                // Handled elsewhere
                break;
            }
                
            case firstCombat:
            {
                // Walk the guard
                if(alienGuardOne.speedDelta >= 50)
                {
                    alienGuardOne.speedDelta = 0;
                    
                    // Handled by spaces
                    if(alienGuardOne.x <= 500 && alienGuardOne.moveSwitch == 'A')
                        bipedController.walkSprite('r', alienGuardOne);
                    else if (alienGuardOne.x >= 500 && alienGuardOne.moveSwitch == 'A')
                    {
                        alienGuardOne.moveSwitch = 'B';
                    }
                    else if (alienGuardOne.x >= 200 && alienGuardOne.moveSwitch == 'B')
                    {
                        bipedController.walkSprite('l', alienGuardOne);
                    }
                    else if (alienGuardOne.x <= 200)
                        alienGuardOne.moveSwitch = 'A';
                }
                else
                {
                    alienGuardOne.speedDelta++;
                }
                
                // Walk the guard
                if(alienGuardTwo.speedDelta >= 50)
                {
                    alienGuardTwo.speedDelta = 0;
                    
                    // Handled by spaces
                    if(alienGuardTwo.x <= 500 && alienGuardTwo.moveSwitch == 'A')
                        bipedController.walkSprite('r', alienGuardTwo);
                    else if (alienGuardTwo.x >= 500 && alienGuardTwo.moveSwitch == 'A')
                    {
                        alienGuardTwo.moveSwitch = 'B';
                    }
                    else if (alienGuardTwo.x >= 200 && alienGuardTwo.moveSwitch == 'B')
                    {
                        bipedController.walkSprite('l', alienGuardTwo);
                    }
                    else if (alienGuardTwo.x <= 200)
                        alienGuardTwo.moveSwitch = 'A';
                }
                else
                {
                    alienGuardTwo.speedDelta++;
                }
                
                // Test for item pickup - knife
                if(human.weapon == 'N')
                {
                    if(alienGuardOne.droppedItem == true)
                    {
                        if(_sdla.checkForCirclePointCollision("player", "knifeStar1"))
                        {
                            _sdla.freeScreenItem("knifeStar1");  // Delete armor pickup
                            _sdla.freeScreenItem("player"); // Delete sprite for player
                            
                            // Dress the player to the correct sprite depending on if they have armor or not
                            if(human.armor == 'N')
                            {
                                (human.gender) ? human.image = "sprites/male_basic_dagger.png" :
                                 human.image = "sprites/female_basic_dagger.png";
                            }
                            else
                            {
                                (human.gender) ? human.image = "sprites/male_armor_dagger.png" :
                                                 human.image = "sprites/female_armor_dagger.png";
                            }
                            
                            // Set Weapon
                            human.weapon = 'D';
                            _sdla.editTextItemMessage("userInfo", "PICKED UP A DAGGER");
                            
                            // Re-draw sprite
                            _sdla.createScreenItem("player", human.image, human.spriteRow, human.spriteColumn, 64, 64, human.x, human.y, 64, 64);
                        }
                    }
                }
                
                // Test for item pickup - armor
                if(human.armor == 'N')
                {
                    if(alienGuardTwo.droppedItem == true)
                    {
                        if(_sdla.checkForCirclePointCollision("player", "armor"))
                        {
                            _sdla.freeScreenItem("armor");  // Delete armor pickup
                            _sdla.freeScreenItem("player"); // Delete sprite for player
                            
                            // Dress the player to the correct sprite depending on if they have armor or not
                            if(human.weapon == 'N')
                            {
                                (human.gender) ? human.image = "sprites/male_armor.png" : human.image = "sprites/female_armor.png";
                            }
                            else
                            {
                                (human.gender) ? human.image = "sprites/male_armor_dagger.png" :
                                human.image = "sprites/female_armor_dagger.png";
                            }
                            
                            // Set Weapon
                            human.armor = 'A';
                            _sdla.editTextItemMessage("userInfo", "PICKED UP ARMOR");
                            
                            // Dress the player
                            _sdla.createScreenItem("player", human.image, human.spriteRow, human.spriteColumn, 64, 64, human.x, human.y, 64, 64);
                        }
                    }
                }
                
                 // Check to switch levelState
                if(human.armor == 'A' && human.weapon == 'D' && allowStoryStep == false && spaceTap == 10)
                {
                    allowStoryStep = true;
                    _sdla.editTextItemVisibility("story1", true);
                    _sdla.editTextItemMessage("story1", "What are you doing?!   ...");
                }
                
                if(spaceTap == 13)
                {
                    _sdla.editScreenItemSetVisibility("portal", true);
                    
                    _sdla.createScreenItem("portalAn", "sprites/teleport_animation.png",
                                           0, 0, 200, 200, GAME_SCREEN_WIDTH/2-100, (GAME_SCREEN_HEIGHT/2)-70, 200, 200);
                    
                }
                break;
            }
                
            case transOut:
            {
                SDL_Rect pl = _sdla.getScreenItemDestinationRectangleData("player");
                SDL_Rect po = _sdla.getScreenItemDestinationRectangleData("portal");
                
                if(checkForPlayerHumanoidColliding(pl, po))
                {
                    _sdla._isRunning = false;
                    SDL_Delay(600);
                }
                break;
            }
        }
        
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
                            _sdla.editScreenItemSetVisibility("inventory", false);
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
                            spaceTap++;
                            // Story progression
                            switch(spaceTap)
                            {
                                case 1:
                                    _sdla.editTextItemMessage("story1", "I am the one who ordered your capture ...");
                                    break;
                                case 2:
                                    _sdla.editTextItemMessage("story1", "Nostradoomoos, a wise man of our people ...");
                                    break;
                                case 3:
                                    _sdla.editTextItemMessage("story1", "Fortold the glory that is our conquest ...");
                                    break;
                                case 4:
                                    _sdla.editTextItemMessage("story1", "Saying we would be victorius only if ...");
                                    break;
                                case 5:
                                    _sdla.editTextItemMessage("story1", "We were to imprision you, as you are ...");
                                    break;
                                case 6:
                                    _sdla.editTextItemMessage("story1", "the ONLY being that could stop us! ...");
                                    break;
                                case 7:
                                {
                                    std::string gender;
                                    (human.gender) ? gender = "him" : gender = "her" ;
                                    _sdla.editTextItemMessage("story1", ("GUARDS! Watch " + gender + " while I fetch the death ray!"));
                                    break;
                                }
                                case 8:
                                {
                                    // Hide the story text, and disable story mode.
                                    _sdla.editTextItemVisibility("story1", false);
                                    allowStoryStep = false;
                                    break;
                                }
                                case 9:
                                    _sdla.editTextItemMessage("story1", "Walk up to the guard, and press SPACE until its dead.");
                                    break;
                                case 10:
                                {
                                    _sdla.editTextItemMessage("story1", "Good luck!");
                                    _sdla.editTextItemVisibility("story1", false);
                                    allowStoryStep = false;                             // Disable story mode
                                    levelState = firstCombat;                           // Start combat
                                    _sdla.editTextItemMessage("userInfo", "PRESS SPACE TO ATTACK");
                                    doHandleSpriteMovement = true;                      // Let person move
                                    break;
                                }
                                case 11:
                                {
                                    _sdla.editTextItemMessage("story1", "Do you really think you can escape?!");
                                    break;
                                }
                                case 12:
                                {
                                    _sdla.editTextItemMessage("story1", "I WILL END YOU! . . .");
                                    break;
                                }
                                case 13:
                                {
                                    allowStoryStep = true;
                                    _sdla.editTextItemMessage("story1", "[STATIC NOISE] ...");
                                    _sdla.editTextItemMessage("userInfo", "PRESS SPACE TO CONTINUE DIALOG");
                                    break;
                                }
                                case 14:
                                {
                                    _sdla.editTextItemMessage("story1", "[VOICE IN HEAD] This is Nostradoomoos...");
                                    _sdla.editTextItemMessage("userInfo", "PRESS SPACE TO CONTINUE DIALOG");
                                    break;
                                }
                                case 15:
                                {
                                    _sdla.editTextItemMessage("story1", "[Nostradoomoos] I've opened a portal to get you out ...");
                                    break;
                                }
                                case 16:
                                {
                                    _sdla.editTextItemMessage("story1", "[Nostradoomoos] of the room of death, but ...");
                                    break;
                                }
                                case 17:
                                {
                                    _sdla.editTextItemMessage("story1", "[Nostradoomoos] I can only move you up one level ...");
                                    break;
                                }
                                case 18:
                                {
                                    _sdla.editTextItemMessage("story1", "[Nostradoomoos] After that you will have to find me ...");
                                    break;
                                }
                                case 19:
                                {
                                    allowStoryStep = false;
                                    _sdla.editTextItemVisibility("story1", false);
                                    _sdla.editTextItemMessage("userInfo", "STEP INTO THE PORTAL");
                                    levelState = transOut;
                                    break;
                                }
                                default:
                                    std::cout << "If you are reading this Josh messed up on level one. Fail him.\n";
                                    break;
                            }
                        }
                        else
                        {
                            // Combat detection!!!
                            // find which way they are facing, and collision detect AOE against potential enemies
                            SDL_Rect hit = attackInFacingDirection(human);
                            if(checkForItemObjectColliding(hit))
                            {
                                //std::cout << "HIT  ALIEN : " << itemThatCollidedWithPlayer << std::endl;
                                
                                // Check for damage to alien 1
                                if(itemThatCollidedWithPlayer == '1')
                                {
                                    if(human.image == "sprites/male_basic_dagger.png" || human.image == "sprites/male_armor_dagger.png" ||
                                       human.image == "sprites/female_basic_dagger.png" || human.image == "sprites/female_armor_dagger.png")
                                    {
                                        alienGuardOne.health -= 3;
                                    }
                                    else
                                        alienGuardOne.health -= 1;
                                }
                                
                                // Check for damage to alien2
                                if(itemThatCollidedWithPlayer == '2')
                                {
                                    if(human.image == "sprites/male_basic_dagger.png" || human.image == "sprites/male_armor_dagger.png" ||
                                        human.image == "sprites/female_basic_dagger.png" || human.image == "sprites/female_armor_dagger.png")
                                    {
                                        alienGuardTwo.health -= 3;
                                    }
                                    else
                                        alienGuardTwo.health -= 1;
                                }
                                
                                // Check for alien 1 death
                                if(alienGuardOne.health <= 0)
                                {
                                    if(!alienGuardOne.droppedItem)
                                    {
                                        _sdla.editScreenItemDestinationXY("knifeStar1", alienGuardOne.x, alienGuardOne.y);
                                        _sdla.editScreenItemSetVisibility("knifeStar1", true);
                                        alienGuardOne.droppedItem = true;
                                    }
                                    
                                    _sdla.editScreenItemSetVisibility("alien1", false);
                                }
                                
                                // Check for alien 2 death
                                if(alienGuardTwo.health <= 0)
                                {
                                    if(!alienGuardTwo.droppedItem)
                                    {
                                        _sdla.editScreenItemDestinationXY("armor", alienGuardTwo.x, alienGuardTwo.y);
                                        _sdla.editScreenItemSetVisibility("armor", true);
                                        alienGuardTwo.droppedItem = true;
                                    }
                                    
                                    _sdla.editScreenItemSetVisibility("alien2", false);
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
