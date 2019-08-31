//
//  biped.hpp
//  Game_321
//
//  Created by Josh Bosley on 10/14/16.
//  Copyright © 2016 Josh Bosley. All rights reserved.
//

#ifndef biped_hpp
#define biped_hpp

#include <stdio.h>

#include "common.h"
#include <iostream>

class biped
{
public:
    biped();
    
    void walkSprite(char dir, humanoid &b, bool diableBounds = false);

    
};

#endif /* biped_hpp */
