//
//  CSCI 321
//  Assignment 1
//  Created by Josh Bosley on 8/30/16.
//

// The width and height of the screen to display
#define SCREEN_WIDTH    500
#define SCREEN_HEIGHT   500

#include <iostream>
#include <SDL2/SDL.h>

int main(int argc, const char * argv[])
{
    // When to quit
    bool running = true;
    
    // The window
    SDL_Window* window;
    
    // Attempt initialization!
    if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
    {
        // Because I don't exist in the standard namespace!!!
        std::cout << "Failed to init, here is the error though! "
                  << std::endl << SDL_GetError() << std::endl;
    }
    else
    {
        // Create the window !
        window = SDL_CreateWindow( "SQUARES!", SDL_WINDOWPOS_UNDEFINED,
                                    SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH,
                                    SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
        
        // If the window couldn't be created, we need to error out and avoid the actual running.
        if(!window)
        {
            std::cout << "Window could not be created! "
            << std::endl << SDL_GetError() << std::endl;
        }
        else
        {
            // Create an event. Doesn't matter what it is named.
            SDL_Event moot;
            
            // Create teh renderer, and set the flag that I'm not to sure what it is or why it
            // works at the moment. All I know is that I tried other flags, and there were issues.
            SDL_Renderer * rend;
            rend = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);

            // This sets the bg color to some green nasty
            SDL_SetRenderDrawColor(rend, 150, 225, 0, 255);
            SDL_RenderClear(rend);
            
            // The primary action-loop that controls everything
            while(running)
            {
                // Render whatever the renderer has to render everytime.
                SDL_RenderPresent(rend);
                
                // Pole for events that occur
                while(SDL_PollEvent(&moot))
                {
                    // Look for the type
                    switch(moot.type)
                    {
                        // If a mouse button down is found, we know we are on the right track
                        case SDL_MOUSEBUTTONDOWN:
                        {
                            // If we are in the danger zone, exit. (As-per assignment rules)
                            if((moot.button.x < 50) && (moot.button.y < 50))
                                running = false;
                            
                            // Create a rectangle!
                            SDL_Rect r;
                            
                            // Set the x, and y ever so slightly off as-to place the square's center on the mouse
                            r.x = moot.button.x-25;
                            r.y = moot.button.y-25;
                            
                            // Width and height of the square
                            r.w = 50;
                            r.h = 50;
                            
                            // Set the color of the square (some dark blue)
                            SDL_SetRenderDrawColor(rend, 0, 0, 100, 100);
                            
                            // Tell the renderer to fill the rectangle (Will be drawn at the top of the running loop)
                            SDL_RenderFillRect(rend, &r);
                            break;
                        }
                        // Close button
                        case SDL_QUIT:
                        {
                            running = false;
                            break;
                        }
                    } // End switch
                } // End Event polling
            } // End 'running'
        } // End if-else
    } // End if-else
    // Clean-up Everything.
    SDL_DestroyWindow( window );
    SDL_Quit();
    return 0;
}
