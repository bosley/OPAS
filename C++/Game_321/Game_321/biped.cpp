//
//  biped.cpp
//  Game_321
//
//  Created by Josh Bosley on 10/14/16.
//  Copyright © 2016 Josh Bosley. All rights reserved.
//

#include "biped.hpp"


biped::biped()
{
    
}

void biped::walkSprite(char dir, humanoid &b, bool disableBounds)
{
    switch (dir) {
        case 'u':
            // 8th row
            b.spriteRow = 8 * 64;
            (b.spriteColumn == (8 * 64)) ? b.spriteColumn = 0 : b.spriteColumn += 64;
            
            // Do bound check, and move
            if( disableBounds || b.y > 0)
            {
                b.y -= PLAYER_MOVEMENT_SPEED;
                b.facing = 'u';
            }
            break;
            
        case 'd':
            // 10th row
            b.spriteRow = 10 * 64;
            (b.spriteColumn == (8 * 64)) ? b.spriteColumn = 0 : b.spriteColumn += 64;
            
            // Do bound check, and move
            if( disableBounds || b.y <= (GAME_SCREEN_HEIGHT-(64+58)))
            {
                b.y += PLAYER_MOVEMENT_SPEED;
                b.facing = 'd';
            }
            break;
            
        case 'l':
            // 9th row
            b.spriteRow = 9 * 64;
            (b.spriteColumn == (8 * 64)) ? b.spriteColumn = 0 : b.spriteColumn += 64;
            
            // Do bound check, and move
            if( disableBounds || b.x > 0)
            {
                b.x -= PLAYER_MOVEMENT_SPEED;
                b.facing = 'l';
            }
            break;
            
        case 'r':
            // 11th row
            b.spriteRow = 11 * 64;
            (b.spriteColumn == (8 * 64)) ? b.spriteColumn = 0 : b.spriteColumn += 64;
            
            // Do bound check, and move
            if( disableBounds || b.x < (GAME_SCREEN_WIDTH-64))
            {
                b.x += PLAYER_MOVEMENT_SPEED;
                b.facing = 'r';
            }
            break;
            
        default:
            break;
    }
}
