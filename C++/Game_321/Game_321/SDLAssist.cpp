//
//  SDLAssist.cpp
//  SDLAssist
//
//  Created by Josh Bosley on 9/23/16.
//  Copyright © 2016 Josh.Bosleybosley. All rights reserved.
//

#include "SDLAssist.hpp"

// Constructor
SDLAssist::SDLAssist()
{
    // Init values
    _isRunning = true;
    _verbose = true;
    colorSettings = new drawColor(0,0,0,0);
}

// Deconstructor
SDLAssist::~SDLAssist()
{
    // Clean up, clean up, every malloc evereywhere
    SDL_DestroyRenderer(_renderer);
    SDL_DestroyWindow(_window);
    
   // free(colorSettings);
    
}

/*
 
        PUBLIC
 
 */

// Setup and creations

/*                  Setup window and renderer                      */
void SDLAssist::setupSDL(unsigned height, unsigned width, std::string title)
{
    // Init SDL with all potentially used flags
    if (SDL_Init(SDL_INIT_TIMER|SDL_INIT_AUDIO|SDL_INIT_VIDEO) != 0)
        createError(SDLASSIST_ERROR_FATAL, ("Failed to init SDL : " + std::string(SDL_GetError())));
    // Create window and renderer
    SDL_CreateWindowAndRenderer(width, height, SDL_WINDOW_SHOWN, &_window, &_renderer);
    // Set title
    SDL_SetWindowTitle(this->_window, title.c_str());
    // Ensure they are created
    if (_window == NULL)
        createError(SDLASSIST_ERROR_FATAL, ("Failed to init Window : " + std::string(SDL_GetError())));
    if (_renderer == NULL)
        createError(SDLASSIST_ERROR_FATAL, ("Failed to init Renderer : " + std::string(SDL_GetError())));
    
    // Init the TTF
    if(-1 == TTF_Init())
    {
        createError(SDLASSIST_ERROR_FATAL, ("Failed to init TTL." ));
        exit(1);
    }
}

/*                  Create an item to display on the screen         */
void SDLAssist::createScreenItem(std::string name, std::string filePath, unsigned int sourceX, unsigned int sourceY, unsigned int sourceHeight, unsigned int sourceWidth, unsigned int screenLocX, unsigned int screenLocY, unsigned int screenLocHeight, unsigned int screenLocWidth)
{
    // Create an item
    screenItem si(name, filePath, sourceX, sourceY, sourceHeight, sourceWidth,
                screenLocX, screenLocY, screenLocHeight, screenLocWidth);
    // Load the image
    si.image = imageLoad(si.path.c_str());
    // Add it to the vector
    this->_screenItems.push_back(si);
}

/*                  Create some text                                */
void SDLAssist::createTextItem(std::string itemsName, std::string messageText, std::string messageFont, SDL_Color fontColor,
                               int x, int y, int h, int w)
{
    textObject to;
    to.visible = true;
    to.text = messageText;
    to.itemName = itemsName;
    to.textColor = fontColor;
    to.font = TTF_OpenFont(messageFont.c_str(), 100);
    to.mRect.x = x; to.mRect.y = y; to.mRect.h = h; to.mRect.w = w;
    to.ttfSurface = TTF_RenderText_Solid(to.font, messageText.c_str(), fontColor);
    to.ttfTexture = SDL_CreateTextureFromSurface(_renderer, to.ttfSurface);
    _textItems.push_back(to);
}

/*                  Draw a line                                     */
void SDLAssist::createLineSegment(std::string label, fPoint a, fPoint b)
{
    _lineSegments.push_back(lineSegment(a,b,label));
}


void SDLAssist::editLineSegment(std::string name, fPoint a, fPoint b)
{
    for (std::vector<lineSegment>::iterator it = _lineSegments.begin() ; it != _lineSegments.end(); ++it)
        if(name == (*it).name){
            (*it).one = a;
            (*it).two = b;
        }
}

void SDLAssist::freeLineSegment(std::string name)
{
    // Remove the item out of the vector YAY!
    _lineSegments.erase(
                       std::remove_if(_lineSegments.begin(), _lineSegments.end(), [&](lineSegment const & si) {
        return si.name == name;
    }),_lineSegments.end());
}

/*                  Manually edit images to be displayed            */
void SDLAssist::editScreenItemSourceXY(std::string name, unsigned newX, unsigned newY)
{
    for (std::vector<screenItem>::iterator it = _screenItems.begin() ; it != _screenItems.end(); ++it)
        if(name == (*it).name){
            (*it).source.x = newX;
            (*it).source.y = newY;
        }
}

/*                  Manually edit images to be displayed            */
void SDLAssist::editScreenItemSourceHW(std::string name, unsigned newH, unsigned newW)
{
    for (std::vector<screenItem>::iterator it = _screenItems.begin() ; it != _screenItems.end(); ++it)
        if(name == (*it).name){
            (*it).source.h = newH;
            (*it).source.w = newW;
        }
}

/*                  Manually edit images to be displayed            */
void SDLAssist::editScreenItemDestinationXY(std::string name, unsigned newX, unsigned newY)
{
    for (std::vector<screenItem>::iterator it = _screenItems.begin() ; it != _screenItems.end(); ++it)
        if(name == (*it).name){
            (*it).destination.x = newX;
            (*it).destination.y = newY;
        }
}

/*                  Manually edit images to be displayed            */
void SDLAssist::editScreenItemDestinationHW(std::string name, unsigned newH, unsigned newW)
{
    for (std::vector<screenItem>::iterator it = _screenItems.begin() ; it != _screenItems.end(); ++it)
        if(name == (*it).name){
            (*it).destination.h = newH;
            (*it).destination.w = newW;
        }
}

/*                  Change an image's visibility                    */
void SDLAssist::editScreenItemSetVisibility(std::string name, bool state)
{
    for (std::vector<screenItem>::iterator it = _screenItems.begin() ; it != _screenItems.end(); ++it)
        if(name == (*it).name){
            if(state)
                (*it).enabled = 'T';
            else
                (*it).enabled = 'F';
        }
}

/*                  Rotate an Image                                 */
void SDLAssist::rotateScreenItem(std::string screenItemOne, double angle)
{
    bool fo = false;
    std::vector<screenItem>::iterator sio;
    for(std::vector<screenItem>::iterator it = _screenItems.begin(); it != _screenItems.end(); ++it){
        if((*it).name == screenItemOne){
            sio = it;
            (*it).enabled = 'R';    // Freeze copying to standard renderer
            fo = true;
        }
    }
    if(!fo)
    {
        if(_verbose)
            createError(SDLASSIST_ERROR_VERBOSE_WARNING, ("Failed to find image to rotate : " + std::string(SDL_GetError())));
        return;
    }
    (*sio).angle = angle;
}


/*                  Remove image from render list                   */
void SDLAssist::freeScreenItem(std::string name)
{
    // Destory the texture to avoid a SERIOUS memory leak ;)
    for (std::vector<screenItem>::iterator it = _screenItems.begin() ; it != _screenItems.end(); ++it)
        if(name == (*it).name){
                    SDL_DestroyTexture((*it).image);
        }
    // Remove the item out of the vector YAY!
    _screenItems.erase(
               std::remove_if(_screenItems.begin(), _screenItems.end(), [&](screenItem const & si) {
        return si.name == name;
    }),_screenItems.end());
}

/*                  Edit a text item                                   */
void SDLAssist::editTextItemMessage(std::string itemName, std::string newMessage)
{
    for(std::vector<textObject>::iterator it = _textItems.begin(); it != _textItems.end(); ++it){
        if ((*it).itemName == itemName) {
            (*it).text = newMessage;
        }
    }
}

/*                  Edit a text item                                   */
void SDLAssist::editTextItemLocation(std::string itemName, int x, int y)
{
    for(std::vector<textObject>::iterator it = _textItems.begin(); it != _textItems.end(); ++it){
        if ((*it).itemName == itemName) {
            (*it).mRect.x = x;
            (*it).mRect.y = y;
        }
    }
}

/*                  Edit a text item height and width                  */
void SDLAssist::editTextItemHW(std::string itemName, int h, int w)
{
    for(std::vector<textObject>::iterator it = _textItems.begin(); it != _textItems.end(); ++it){
        if ((*it).itemName == itemName) {
            (*it).mRect.h = h;
            (*it).mRect.w = w;
        }
    }
}

/*                  Edit a text item visibiity                         */
void SDLAssist::editTextItemVisibility(std::string itemName, bool visibility)
{
    for(std::vector<textObject>::iterator it = _textItems.begin(); it != _textItems.end(); ++it){
        if ((*it).itemName == itemName) {
            (*it).visible = visibility;
        }
    }
}

/*                  Free a text item                                   */
void SDLAssist::freeTextItem(std::string itemName)
{
    std::vector<textObject>::iterator si;
    for(std::vector<textObject>::iterator it = _textItems.begin(); it != _textItems.end(); ++it){
        if ((*it).itemName == itemName) {
            SDL_DestroyTexture((*it).ttfTexture);
            SDL_FreeSurface((*it).ttfSurface);
            free((*it).font);
        }
    }
    
    // Erase from text vector
    _textItems.erase(
                       std::remove_if(_textItems.begin(), _textItems.end(), [&](textObject const & si) {
        return si.itemName == itemName;
    }),_textItems.end());
    
}


/*                  Check for SP collision between two screen items    */
bool SDLAssist::checkForSquarePointCollision(std::string screenItemOne, std::string screenItemTwo)
{
    bool hasCollided = false, fo = false, ft = false;
    
    // Get Item One
    std::vector<screenItem>::iterator sio, sit;
    for(std::vector<screenItem>::iterator it = _screenItems.begin(); it != _screenItems.end(); ++it){
        if((*it).name == screenItemOne){
            sio = it;
            fo = true;
        }
    }
    
    // Get Item Two
    for(std::vector<screenItem>::iterator it = _screenItems.begin(); it != _screenItems.end(); ++it){
        if((*it).name == screenItemTwo){
            sit = it;
            ft = true;
        }
    }
    
    // Make sure the item is valid before continuing
    if(!fo || !ft)
        return false;
    
    // At this point it is assumed that they are found. There is no error checking for not finding an image
    if(((*sio).destination.w > (*sit).destination.w) && ((*sio).destination.h > (*sit).destination.h))
    {
        // Screen item one is bigger in h and w
        /*
            Sio
            ------  sit
            |    |  ----
            |    |  |  |
            ------  ----
         */
        
        // Test if screen item two's top left corner is within screen items one's x range
        if( ((*sit).destination.x > (*sio).destination.x) &&
            ((*sit).destination.x < ((*sio).destination.x + (*sio).destination.w )))
        {
            // Test y boundaries
            if( ((*sit).destination.y > (*sio).destination.y) &&
                ((*sit).destination.y < ((*sio).destination.y + (*sio).destination.h)))
            {
                //std::cout << "CORNER OF SMALLER ITEM IS INSIDE BIGGER ITEM\n";
                return true;
            }
        }
    }
    else if (((*sio).destination.w > (*sit).destination.w) && ((*sio).destination.h < (*sit).destination.h))
    {
        // Screen item one is wider, but height is smaller
        /* 
                       sit
              sio     ----
            --------  |  |
            |      |  |  |
            --------  ----
         
         */
        if( ((*sio).destination.x > (*sit).destination.x) &&
           ((*sio).destination.x < ((*sit).destination.x + (*sit).destination.w )))
        {
            // Test y boundaries
            if( ((*sio).destination.y > (*sit).destination.y) &&
               ((*sio).destination.y < ((*sit).destination.y + (*sit).destination.h)))
            {
                return true;
            }
        }
    }
    else if (((*sio).destination.w < (*sit).destination.w) && ((*sio).destination.h > (*sit).destination.h))
    {
        // Screen item isn't as wide, but its height is greater
        
        /*  sio
            ----  sit
            |  |  --------
            |  |  |      |
            ----  --------
         */
        
        if( ((*sit).destination.x > (*sio).destination.x) &&
           ((*sit).destination.x < ((*sio).destination.x + (*sio).destination.w )))
        {
            // Test y boundaries
            if( ((*sit).destination.y > (*sio).destination.y) &&
               ((*sit).destination.y < ((*sio).destination.y + (*sio).destination.h)))
            {
                //std::cout << "CORNER OF SMALLER ITEM IS INSIDE BIGGER ITEM\n";
                return true;
            }
        }
    }
    else if (((*sio).destination.w < (*sit).destination.w) && ((*sio).destination.h < (*sit).destination.h))
    {
        // Screen item one is smaller in both respects
        /*
                    Sit
           sio     ------
           ---     |    |
          |   |    |    |
           ---     ------
         */
        // Test if screen item one's top left corner is within screen items two's x range
        if( ((*sio).destination.x > (*sit).destination.x) &&
           ((*sio).destination.x < ((*sit).destination.x + (*sit).destination.w )))
        {
            // Test y boundaries
            if( ((*sio).destination.y > (*sit).destination.y) &&
               ((*sio).destination.y < ((*sit).destination.y + (*sit).destination.h)))
            {
                return true;
            }
        }
    }
    return hasCollided;
}

/*                  Check for CC collision between two screen items    */
bool SDLAssist::checkForCircleCircleCollision(std::string screenItemOne, std::string screenItemTwo)
{
    bool fo = false, ft = false;
    // Get Item One
    std::vector<screenItem>::iterator sio, sit;
    for(std::vector<screenItem>::iterator it = _screenItems.begin(); it != _screenItems.end(); ++it){
        if((*it).name == screenItemOne){
            sio = it;
            fo = true;
        }
    }
    // Get Item Two
    for(std::vector<screenItem>::iterator it = _screenItems.begin(); it != _screenItems.end(); ++it){
        if((*it).name == screenItemTwo){
            sit = it;
            ft = true;
        }
    }
    // Make sure the item is valid before continuing
    if(!fo || !ft)
        return false;
    
    // Compute distance^2 =     (y2 - y1)^2 + (x2 - x1)^2
    int distanceSquared = pow((((*sit).destination.y + ((*sit).destination.h/2)) -
                              ((*sio).destination.y + ((*sio).destination.h/2))),2);
       distanceSquared += pow((((*sit).destination.x + ((*sit).destination.w/2)) - ((*sio).destination.x + ((*sio).destination.w/2))),2);
    
    // Computer screen item one radius + screen item two radius ^ 2
    int radiiSquared = pow((((*sio).destination.w/2) + ((*sit).destination.w/2)),2);
    
    // If the distance between the two points is less than
    if(radiiSquared > distanceSquared)
        return true;
    else
        return false;
}

/*                  Check for CPC collision between two screen items    */
bool SDLAssist::checkForCirclePointCollision(std::string circleObject, std::string pointObject)
{
    bool fo = false, ft = false;
    // Get Item One
    std::vector<screenItem>::iterator sio, sit;
    for(std::vector<screenItem>::iterator it = _screenItems.begin(); it != _screenItems.end(); ++it){
        if((*it).name == circleObject){
            sio = it;
            fo = true;
        }
    }
    // Get Item Two
    for(std::vector<screenItem>::iterator it = _screenItems.begin(); it != _screenItems.end(); ++it){
        if((*it).name == pointObject){
            sit = it;
            ft = true;
        }
    }
    // Make sure the item is valid before continuing
    if(!fo || !ft)
        return false;
    int radius = (*sio).destination.w/2;                                // Get radius of the circle
    int radiusSquared = pow(radius, 2);                                 // Square it
    int centerX = (*sio).destination.x + ((*sio).destination.w/2);      // Get center x,y of the circle
    int centerY = (*sio).destination.y + ((*sio).destination.h/2);
    int distanceSquared = pow(((*sit).destination.y - centerY ), 2);    // Square the distance from point to
    distanceSquared += pow(((*sit).destination.x - centerX ),2);
    if(radiusSquared > distanceSquared)
        return true;
    else
        return false;
}

/*                  Check for point in triangle collision                */
bool SDLAssist::checkForTrianglePointCollision(float pointx, float pointy, float tp1x, float tp1y,
                                               float tp2x, float tp2y, float tp3x, float tp3y)
{
    // Check to see which side the point in question (piq) is relative to 1, 2
    bool side1 = (pointx-tp2x) * (tp1y-tp2y) - (tp1x-tp2x) * (pointy-tp2y) < 0.0;
    // Check to see which side the piq is relative to 2, 3
    bool side2 = (pointx-tp3x) * (tp2y-tp3y) - (tp2x-tp3x) * (pointy-tp3y) < 0.0;
    // Check to see which side the piq is relative to 1, 3, YAY CIRCUIT!
    bool side3 = (pointx-tp1x) * (tp3y-tp1y) - (tp3x-tp1x) * (pointy-tp1y) < 0.0;
    // Return true if they are all the same!
    return ((side1 == side2) && (side2 == side3));
}

/*                  Retrieve SDL_Rect Information                       */
SDL_Rect SDLAssist::getScreenItemSourceRectangleData(std::string screenItemName)
{
    for(std::vector<screenItem>::iterator it = _screenItems.begin(); it != _screenItems.end(); ++it){
        if((*it).name == screenItemName){
            return (*it).source;
        }
    }
    SDL_Rect a; // Silence the error. May be bad practice. Hopefully the programmer realized they need to search
    return a;   // for items that exist in the screenItem vector. . .
}

/*                  Retrieve SDL_Rect information                       */
SDL_Rect SDLAssist::getScreenItemDestinationRectangleData(std::string screenItemName)
{
    for(std::vector<screenItem>::iterator it = _screenItems.begin(); it != _screenItems.end(); ++it){
        if((*it).name == screenItemName){
            return (*it).destination;
        }
    }
    SDL_Rect a; // Silence the error. May be bad practice. Hopefully the programmer realized they need to search
    return a;   // for items that exist in the screenItem vector. . . 
}

/*                  Retrieve Visibility Information                      */
bool SDLAssist::getScreenItemVisibility(std::string screenItemName)
{
    for(std::vector<screenItem>::iterator it = _screenItems.begin(); it != _screenItems.end(); ++it){
        if((*it).name == screenItemName){
            return ((*it).enabled == 'T');
        }
    }
    return false;
}

/*                  Retrieve Visibility Information                      */
bool SDLAssist::getTextItemVisibility(std::string itemName)
{
    for(std::vector<textObject>::iterator it = _textItems.begin(); it != _textItems.end(); ++it){
        if ((*it).itemName == itemName) {
            return ((*it).visible);
        }
    }
    return false;
}

/*                  Set the draw color of the renderer              */
void SDLAssist::setRenderDrawColor(int red, int green, int blue, int alpha)
{
    this->red = red;
    this->green = green;
    this->blue = blue;
    this->alpha = alpha;
}

/*                  Update the screen                               */
void SDLAssist::updateScreen(bool renderClear, bool renderItems, bool renderDrawColor)
{
    // Render, draw things that are set to be rendered or drawn
    if(renderClear)
        SDL_RenderClear(_renderer);
    
    if(renderItems)
        renderScreenItems();
    
    if(renderDrawColor)
    {
     //   std::cout << "RENDER DRAW COLOR : " << red << green << blue << std::endl;
        SDL_SetRenderDrawColor(_renderer,
                               this->red, this->green, this->blue, this->alpha);
    }
    
    // Tell SDL to present what is in the renderer
    SDL_RenderPresent(_renderer);
}

/*
 
            PRIVATE
 
 */

/*                  Create an error                                */
void SDLAssist::createError(char code, std::string message)
{
    errorInfo ei(code, message);
    this->_errors.push(ei);
    if(SDLASSIST_ERROR_FATAL == code){
        std::cout << "Fatal error detected in SDLAssist. Enable verbose to display error messages.\n";
        while(this->_verbose && !this->_errors.empty()){
            std::cout << this->_errors.top().message << std::endl;
            this->_errors.pop();
        }
        exit(1);
    }
}

/*                  Add all vector screen itmes to renderer*/
void SDLAssist::renderScreenItems()
{
    // First come first rendered - Rendering of images
    for(int i = 0; i < this->_screenItems.size(); ++i)
    {
        if('T' == _screenItems[i].enabled)
        {
            SDL_RenderCopy(this->_renderer,
                       this->_screenItems[i].image,
                       &_screenItems[i].source, &_screenItems[i].destination);
        }
        else if ('R' ==_screenItems[i].enabled)
        {
            // Handle an object that is rotate/rotating
            SDL_RendererFlip f = SDL_FLIP_NONE;
            SDL_Rect s = _screenItems[i].source;
            SDL_Rect d = _screenItems[i].destination;
            int r = SDL_RenderCopyEx(_renderer, _screenItems[i].image, &s, &d, _screenItems[i].angle, NULL, f);
            if(0 != r)
            {
                createError(SDLASSIST_ERROR_FATAL, ("Failed to rotate : " + std::string(SDL_GetError())));
            }
        }
    }
    
    // Render the text objects onto the screen
    for(int i = 0; i < this->_textItems.size(); ++i)
    {
        if(this->_textItems[i].visible)
        {
            // Update the item, and render
            SDL_DestroyTexture(_textItems[i].ttfTexture); // Perform some clean up
            SDL_FreeSurface(_textItems[i].ttfSurface);    // MMMAAAASSSSIVE mem leak if not done
            _textItems[i].ttfSurface = TTF_RenderText_Solid(_textItems[i].font, _textItems[i].text.c_str(), _textItems[i].textColor);
            _textItems[i].ttfTexture = SDL_CreateTextureFromSurface(_renderer, _textItems[i].ttfSurface);
            SDL_RenderCopy(_renderer, _textItems[i].ttfTexture, NULL, &_textItems[i].mRect);
        }
    }
    
    // Render lines
    for(int i = 0; i < this->_lineSegments.size(); ++i)
    {
        drawLine(this->_lineSegments[i].one, this->_lineSegments[i].two);
    }
    
    
    
}

/*                  Create an image                                */
SDL_Texture* SDLAssist::imageLoad(const char* filename)
{
    SDL_Texture *texture;
    texture = IMG_LoadTexture(_renderer, filename);
    if (texture == NULL)
        createError(SDLASSIST_ERROR_FATAL,
                    ("Failed to load image [ " +
                     std::string(filename) +
                     " ]" +
                     std::string(IMG_GetError())));
    return texture;
}

/*                  Stop everything                                 */
void SDLAssist::killSdlAssist()
{
    // Destory images
    for (std::vector<screenItem>::iterator it = _screenItems.begin() ; it != _screenItems.end(); ++it)
    {
        SDL_DestroyTexture((*it).image);
    }
    // Clear the vector
    _screenItems.clear();
    
    // Destroy texts
    std::vector<textObject>::iterator si;
    for(std::vector<textObject>::iterator it = _textItems.begin(); it != _textItems.end(); ++it)
    {
        SDL_DestroyTexture((*it).ttfTexture);
        SDL_FreeSurface((*it).ttfSurface);
        free((*it).font);
    }
    
    // Clean up everything else
    SDL_DestroyRenderer(_renderer);
    SDL_DestroyWindow(_window);
    free(colorSettings);
}


void SDLAssist::fillPixel(fPoint point)
{
    {
        SDL_Rect pixel;
        pixel.x = point.x; pixel.y = point.y;
        pixel.w = 2; pixel.h = 2;
        SDL_SetRenderDrawColor(_renderer,
                               255, 0, 0, 0);
        
        SDL_RenderFillRect(_renderer, &pixel);
    }
}

void SDLAssist::drawLine(fPoint one, fPoint two)
{
    float x1 = one.x, x2 = two.x, y1 = one.y, y2 = two.y;
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
    int y = (int)y1;
    
    for(int x = (int)x1; x < (int)x2; x++)
    {
        if(isSteep)
        {
            fillPixel(fPoint(y,x));
        }
        else
        {
            fillPixel(fPoint(x,y));
        }
        err -= dy;
        if(err < 0)
        {
            y += yStep;
            err += dx;
        }
    }
}




