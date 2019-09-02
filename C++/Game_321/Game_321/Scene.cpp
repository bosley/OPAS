//
//  Scene.cpp
//  Game_321
//
//  Created by Josh Bosley on 10/12/16.
//  Copyright © 2016 Josh Bosley. All rights reserved.
//

#include "Scene.hpp"

Scene::Scene()
{
    std::cout << "Scene init" << std::endl;
}

sceneFrame Scene::getCurrentSceneFrame()
{
    // Grab a scene, remove it, give it away!
    sceneFrame s = this->frames.front();
    this->frames.pop();
    return s;
}

void Scene::interruprSceneWithCode(char code)
{
    std::cout << "Recieved interrupt: " << code << std::endl;
}
