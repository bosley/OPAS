//
//  common.h
//  Game_321
//
//  Created by Josh Bosley on 10/12/16.
//  Copyright © 2016 Josh Bosley. All rights reserved.
//

#ifndef common_h
#define common_h

#include "Scene.hpp"
#include "SDLAssist.hpp"
#include <iostream>

// Place sprite width.height and all that here

#define GAME_SCREEN_WIDTH 800
#define GAME_SCREEN_HEIGHT 600
#define PLAYER_MOVEMENT_SPEED 10

// Information about the player
struct humanoid{
    std::string image;
    std::string itemName;
    double x, y, health;
    int spriteRow, spriteColumn;
    bool gender; // F = F, T = M
    char facing;          // Which way is the sprite facing
    long speedDelta, hitDelta;
    char moveSwitch;
    bool droppedItem;
    char armor, weapon;
    bool win, lose;
    bool inCombat;
};

struct screenObject{
    std::string name;
    int x, y, h, w;
    bool interactAble;  // Is it interactable?
    char condition;     // if-so, under what condition?
};

// Projectiles that pylons and bosses shoot
struct projectile
{
    double x, y;
    double damage;
    std::string name;
    std::string image;
    double targetX, targetY;
    double cX, cY;
    bool lolflag;
    
    int pindex;
    std::vector<fPoint> path;
    
    projectile();
    projectile(std::string label, std::string image, double ox, double oy, double damage, double tx, double ty, double cx, double cy)
    {
        pindex = 0;
        name = label; this->image = image;
        this->x = ox; this->y = oy; this->damage = damage;
        targetX = tx; targetY = ty;
        this->cX = cx; this->cY = cy;
        
        lolflag = false;
        
        float x1 = ox, x2 = tx, y1 = oy, y2 = ty;
        const bool isSteep = (fabs(y2 - y1)) > (fabs(x2 - x1));
        
        if(isSteep)
        {
            std::swap(x1, y1);
            std::swap(x2, y2);
        }
        
        if(x1 > x2)
        {
            std::swap(x1, x2);
            std::swap(y1, y2);
        }
        
        const float dx = x2 - x1;
        const float dy = fabs(y2 - y1);
        const int yStep = (y1 < y2) ? 1 : -1;
        float err = dx / 2.0;
        int ya = (int)y1;
        
        for(int xa = (int)x1; xa < (int)x2; xa++)
        {
            if(isSteep)
            {
                path.push_back(fPoint(ya,xa));
            }
            else
            {
                path.push_back(fPoint(xa,ya));
            }
            err -= dy;
            if(err < 0)
            {
                ya += yStep;
                err += dx;
            }
        }
    }
};





#endif /* common_h */
