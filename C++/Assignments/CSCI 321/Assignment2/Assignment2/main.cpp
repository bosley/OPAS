//
//  main.cpp
//  Assignment2
//
//  Created by Josh Bosley on 9/2/16.
//  Copyright © 2016 Josh.Bosley. All rights reserved.
//

#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>

// Because Schemm said that globals are okay now . . . .
bool isRunning;
SDL_Window* window;
SDL_Renderer* renderer;

/*              Setup the SDL enviroment    */
void setup()
{
    if (SDL_Init(SDL_INIT_TIMER|SDL_INIT_AUDIO|SDL_INIT_VIDEO) != 0)
    {
        std::cout << "Failed to init SDL :( | Heres the error : " << SDL_GetError() << std::endl;
        exit(1);
    }
    
    // Create window and renderer
    SDL_CreateWindowAndRenderer(600,400, SDL_WINDOW_SHOWN,&window, &renderer);
    
    if (window == NULL)
    {
        std::cout << "Unable to initialize window! " << SDL_GetError() << std::endl;
        exit(1);
    }
    if (renderer == NULL)
    {
        std::cout << "Unable to initialize renderer! " << SDL_GetError() << std::endl;
        exit(1);
    }
}

/*              Load a texture              */
SDL_Texture* imageLoad(const char* filename)
{
    SDL_Texture *texture;
    texture = IMG_LoadTexture(renderer, filename);
    if (texture == NULL)
    {
        std::cout << "Failed to load image [" << filename << "] Error: " << IMG_GetError() << std::endl;
        exit(1);
    }
    return texture;
}

int main(int argc, const char * argv[])
{
    // Attempt setup of SDL enviroment before anything else so we won't waste time if it fails
    setup();
    
    /* Item One */
    
    SDL_Rect fSource, fDest;
    
    int faces_pos = 0;
    
    fSource.x = 0;  fSource.y = 0;
    fSource.w = 50; fSource.h = 50;
    
    fDest.x = 50; fDest.y = 50;
    fDest.w = 50; fDest.h = 50;
    
    SDL_Event event;
    SDL_Texture * faces;
    faces = imageLoad("faces.png");
    
    /* Item Two */
    
    SDL_Rect aSource, aDest;
    
    int a_pos = 0;
    
    aSource.x = 0;  aSource.y = 0;
    aSource.w = 200; aSource.h = 200;
    
    aDest.x = 200; aDest.y = -50;
    aDest.w = 200; aDest.h = 200;
    
    SDL_Texture * awesome;
    awesome = imageLoad("awesome.png");
    
    
    /*  Item 3  */
    
    SDL_Rect tSource, tDest;
    
    int t_pos = 0;
    
    tSource.x = 0;  tSource.y = 0;
    tSource.w = 100; tSource.h = 100;
    
    tDest.x = 150; tDest.y = 150;
    tDest.w = 150; tDest.h = 150;
    
    SDL_Texture * text;
    text = imageLoad("yay.jpg");
    
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    
    isRunning = true;
    
    while(isRunning)
    {
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, faces, &fSource, &fDest);
        SDL_RenderCopy(renderer, text, &tSource, &tDest);
        SDL_RenderCopy(renderer, awesome, &aSource, &aDest);
        SDL_RenderPresent(renderer);
        
        (faces_pos == 4) ? faces_pos = 0 : faces_pos++;
        fSource.x = faces_pos * 50;
        if(fDest.x < 400)
            fDest.x += 10;
        
        if(fDest.x >= 400)
            fDest.y += 10;
        
        if(fDest.y >= 300)
            fDest.y -= 10;
        
        
        (a_pos == 8) ? a_pos = 0 : a_pos++;
        aSource.x = a_pos * 200;
        
        
        (t_pos == 4) ? t_pos = 0 : t_pos++;
        tSource.x = t_pos * 100;
        
        if(tDest.x != 20)
            tDest.x--;
        
        if(aDest.y != 200)
            aDest.y++;
        
   
        if (SDL_PollEvent(&event) > 0)
        {
            switch(event.type)
            {
                case SDL_QUIT:
                    isRunning = false;
                    return 0;
                    break;
            }
        }
        SDL_Delay(50);
    }

    std::cout << "fin\n";
    return 0;
}
