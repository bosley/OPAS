//
//  SDLAssist.hpp
//  SDLAssist
//
//  Created by Josh Bosley on 9/23/16.
//  Copyright © 2016 Josh.Bosleybosley. All rights reserved.
//
//  It may seem like I did my best to obfuscate SDL, but
//  it totally makes managing SDL projects easier in the long run.
//  If you're reading this, you should try it out!
//  The only things requred are the following:
//  SDL2, SDL2_image, SDL2_ttf
//

#ifndef SDLAssist_hpp
#define SDLAssist_hpp

#include <stack>
#include <vector>
#include <string>
#include <iostream>
#include <iterator>

// Sdl libs
#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>
#include <SDL2_ttf/SDL_ttf.h>

// Error codes
#define SDLASSIST_ERROR_FATAL 'X'
#define SDLASSIST_ERROR_VERBOSE_WARNING 'V'

// Error information holding structure
struct errorInfo{
    char code;
    std::string message;
    errorInfo(char c, std::string m){
        code = c; message = m;
    }
};

// Manages the color to 'draw'
struct drawColor{
    Uint8 r, g, b, a;
    drawColor(Uint8 ir, Uint8 ig,Uint8 ib, Uint8 ia){
        r=ir; g=ig; b=ib; a=ia;
    }
};


struct fPoint{
    float x, y;
    fPoint(){x = 0; y = 0;}
    fPoint(float x, float y)
    {
        this->x = x; this->y = y;
    }
    fPoint& operator =(const fPoint &rhs)
    {
        this->x = rhs.x;
        this->y = rhs.y;
        return *this;
    }
};

struct lineSegment{
    std::string name;
    fPoint one, two;
    lineSegment(fPoint a, fPoint b, std::string label)
    {
        name = label;
        this->one = a; this->two = b;
    }
};

// Because every item in SDL takes a bunch of vars to manage, I've decided to
// wrap them up in a struct and stuff them in a vector so I can invoke them by name for
// editing, and whatnot.
struct screenItem{
    char enabled; // Show the image or not
    SDL_Texture* image;
    SDL_Rect source, destination;
    std::string name, path;
    double angle; // If the item id going to be rotated, this will be set.
    screenItem(std::string name, std::string filePath,
                unsigned sourceX, unsigned sourceY, unsigned sourceHeight, unsigned sourceWidth,
                unsigned screenLocX, unsigned screenLocY, unsigned screenLocHeight, unsigned screenLocWidth){
        // Source is the rectangle that will be layed over the img file and used as a frame of ref for getting an image
        source.x = sourceX; source.y = sourceY; source.h = sourceHeight; source.w = sourceWidth;
        // Destination is the rectangle that takes the source stuff, and displays it on screen
        destination.x = screenLocX; destination.y = screenLocY; destination.h = screenLocHeight; destination.w = screenLocWidth;
        path = filePath;
        this->name = name;
        enabled = 'T'; // Enabled by default
    }
};

// Similar to the way I'm handling images, I decided to wrap-up ttf objects
struct textObject{
    std::string text, itemName;     // Text to display, and the name to invoke it
    SDL_Color textColor;            // Color of the text
    SDL_Rect mRect;                 // Rectangle for the text. XY,WH will need to be set
    TTF_Font* font;                 // The font to use.
    SDL_Surface * ttfSurface;       // The surface for the ttf item
    SDL_Texture * ttfTexture;       // The texture for the ttf item
    bool visible;                   // Toggle for the  
};

// The assist class
class SDLAssist
{
public:
    SDLAssist();    // Constructor
    ~SDLAssist();   // Destructor
    
    /*      Setup, and creation methods         */
    void setupSDL(unsigned height, unsigned width, std::string title);              // Setup SDL
    
    void createScreenItem(std::string name, std::string filePath,                   // Create an image
                          unsigned sourceX, unsigned sourceY, unsigned sourceHeight, unsigned sourceWidth,
                          unsigned screenLocX, unsigned screenLocY, unsigned screenLocHeight, unsigned screenLocWidth);
                                                                                    // Create a ttf object
    void createTextItem(std::string itemName, std::string messageText, std::string messageFont, SDL_Color fontColor, int x, int y, int h, int w);
    
    void createLineSegment(std::string label, fPoint a, fPoint b);
    
    /* Draw a line! */
    
    void drawLine(fPoint one, fPoint two);
    
    /*      Edit image items manually           */
    void editScreenItemSourceXY(std::string name, unsigned newX, unsigned newY);     // XY of input
    void editScreenItemSourceHW(std::string name, unsigned newX, unsigned newY);     // HW of input
    void editScreenItemDestinationXY(std::string name, unsigned newX, unsigned newY);// Edit location on screen XY
    void editScreenItemDestinationHW(std::string name, unsigned newH, unsigned newW);// Edit screen HW
    void editScreenItemSetVisibility(std::string name, bool state);                  // Toggle on-off visibility
    void rotateScreenItem(std::string name, double angle);                           // Rotate an image an angle
    void freeScreenItem(std::string name);                                           // Delete, and clean-up
    
    /*      Edit text items                     */
    void editTextItemMessage(std::string originalMessage, std::string newMessage);   // Change the text of a ttf item
    void editTextItemLocation(std::string itemName, int x, int y);                   // Change where to show ttf item
    void editTextItemHW(std::string itemName, int h, int w);                         // Change ttf item HW
    void editTextItemVisibility(std::string itemName, bool visibility);              // Toggle item on-off
    void freeTextItem(std::string itemName);                                         // Delete, and clean-up
    
    /*      Edit line segments                  */
    void editLineSegment(std::string name, fPoint a, fPoint b);
    void freeLineSegment(std::string name);
    
    /*      Collision Detection                 */
    bool checkForSquarePointCollision(std::string screenItemOne, std::string screenItemTwo);  // Check if point in square
    bool checkForCircleCircleCollision(std::string screenItemOne, std::string screenItemTwo); // Check two circles
    bool checkForCirclePointCollision(std::string screenItemOne, std::string screenItemTwo);  // Check if point int circle
    bool checkForTrianglePointCollision(float pointx, float pointy,                           // Check if point in triangle
                                        float tp1x, float tp1y,                               // Put in 3 points of triangle
                                        float tp2x, float tp2y,
                                        float tp3x, float tp3y);

    /*      Retrieve image information          */
    SDL_Rect getScreenItemSourceRectangleData(std::string itemName);                // Get copy of source rectangle
    SDL_Rect getScreenItemDestinationRectangleData(std::string itemName);           // Get copy of destination rectangle
    bool getScreenItemVisibility(std::string itemName);                             // Check if item is visible
    bool getTextItemVisibility(std::string itemName);                               // Check if item is visible
    
    /*      Set the draw color                  */
    void setRenderDrawColor(int red, int green, int blue, int alpha);         // Set the color to render
    
    /*      Control and display                 */
    void updateScreen(bool renderClear, bool renderItems, bool renderDrawColor);      // Tell SDL to update pass in
                                                                                      // pass in optioms
    /*      Disable this assist                 */
    void killSdlAssist();
    
    // Public event control for external handling
    SDL_Event _event;
    
    // Program state for outermost SDL loop
    bool _isRunning;
    
private:
    // Variables
    bool _verbose;                              // Tell if we should yell at the programmer for bug killing
    SDL_Window* _window;                        // The indow that will be presented to the user
    SDL_Renderer* _renderer;                    // The primary renderer
    drawColor* colorSettings;                   // Programmer defined color settings for render draw color
    int red, green, blue, alpha;
    
    // Error Stack
    std::stack<errorInfo> _errors;              // Errors that are detected. Thrown up if fatal error
    
    // Display objects
    std::vector<screenItem> _screenItems;       // Images to display on screen
    
    // Text objects
    std::vector<textObject> _textItems;         // TTF items that are on screen
    
    // Manually Drawn Lines
    std::vector<lineSegment> _lineSegments;
    
    // Internal-use create error.
    void createError(char code, std::string message);   // Internal method to flag an error
    
    // Copies images to renderer
    void renderScreenItems();                     // Internal method to render all objects on the screen
                                                  // This also performs rotations requested by rotateScreenItem
    SDL_Texture* imageLoad(const char* filename); // Internal method to load an image
    
    
    
    void fillPixel(fPoint point);
    
    
};

#endif /* SDLAssist_hpp */
