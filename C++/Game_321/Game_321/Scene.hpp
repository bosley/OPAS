//
//  Scene.hpp
//  Game_321
//
//  Created by Josh Bosley on 10/12/16.
//  Copyright © 2016 Josh Bosley. All rights reserved.
//
/*
        The idea here is to create a 'scene' for an object that can be played out step-by-step 
        this can be used for the control of game objects and their interactions
 
 */
#ifndef Scene_hpp
#define Scene_hpp

#include <Queue>
#include <iostream>
#include "common.h"
#include <stdio.h>

#define SCENE_INTERRUPT_DESTROY_OBJECT 'a'
#define SCENE_INTERRUPT_CHANGE_DIRECTION 'b'
// Insert all codes here


// Information returned from scene of the 'current' state of the game.
// This is passed to the level to render or handle the current state.
struct sceneFrame
{
    
};

// The scene!
class Scene{
    
public:
    Scene();
    
    sceneFrame getCurrentSceneFrame();
    
    // Force priority of something to heppen (destruction), etc)
    void interruprSceneWithCode(char code);
    
private:
    
    std::queue<sceneFrame> frames;
    
};

#endif /* Scene_hpp */
